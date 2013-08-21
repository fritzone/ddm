/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSql module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsql_cubrid.h"
#include <qcoreapplication.h>
#include <qdatetime.h>
#include <qvariant.h>
#include <qsqlerror.h>
#include <qsqlfield.h>
#include <qsqlindex.h>
#include <qsqlquery.h>
#include <qlist.h>
#include <qvector.h>
#include <qtextcodec.h>
#include <qmutex.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

Q_DECLARE_METATYPE(Cubrid_Query_Result *)

QT_BEGIN_NAMESPACE

class QCubridDriverPrivate
{
public:
    QCubridDriverPrivate() : connection(0), isUtf8(false) {}
    Cubrid_Conn connection;
    Cubrid_Error lastError;
    bool isUtf8;
};

class QCubridResultPrivate
{
public:
    QCubridResultPrivate(QCubridResult *qq): q(qq), driver(0), request(0), queryResult(0), columnsInfo(0), currentSize(-1), preparedQueriesEnabled(false) {}

    QCubridResult *q;
    //const QCubridDriverPrivate *driver;
    QCubridDriverPrivate *driver;

    Cubrid_Request request;
    Cubrid_Query_Result *queryResult;
    Cubrid_Columns_Info *columnsInfo;
    Cubrid_Command_Type commandType;
    int ColumnCount;
    int affectedRows;

    int currentSize;
    bool preparedQueriesEnabled;
    QString preparedStmtId;
    QString preparedStmt;

    bool processResults();
};

static QSqlError qMakeError(const QString &err, QSqlError::ErrorType type,
                            const QCubridDriverPrivate *p)
{
    const char *s = p->lastError.err_msg;
    QString msg = p->isUtf8 ? QString::fromUtf8(s) : QString::fromLocal8Bit(s);
    return QSqlError(QLatin1String("QCUBRID: ") + err, msg, type);
}


bool QCubridResultPrivate::processResults()
{
    if (request == 0)
    {
        return false;
    }

    int status = -1;
    if (!preparedStmt.isEmpty())
    {
        // if command was a select
        if (commandType == SQLX_CMD_SELECT)
        {
            status = 0;
        }
    }
    else
    {
        return false;
    }

    if (status == 0)
    {
        q->setSelect(true);
        q->setActive(true);
        if (cci_fetch_size(request, currentSize) != 0)
        {
            currentSize = -1;
        }
        else
        {
            //currentSize = affectedRows;
            //q->fetchFirst(); // FLD: commented out
            q->setAt(QSql::BeforeFirstRow); // FLD: added
        }
        return true;
    }
    else
    {
        q->setSelect(false);
        q->setActive(true);
        currentSize = -1;
        return true;
    }
    q->setLastError(qMakeError(QCoreApplication::translate("QCubridResult", "Unable to create query"),
                               QSqlError::StatementError, driver));
    return false;
}

static QVariant::Type qDecodeCubridType(T_CCI_U_TYPE t)
{
    QVariant::Type type = QVariant::Invalid;
    switch (t)
    {
    case CCI_U_TYPE_BIT:
        type = QVariant::Bool;
        break;
    case CCI_U_TYPE_BIGINT:
        type = QVariant::LongLong;
        break;
    case CCI_U_TYPE_INT:
    case CCI_U_TYPE_SHORT:
        type = QVariant::Int;
        break;
    case CCI_U_TYPE_NUMERIC:
    case CCI_U_TYPE_MONETARY:
    case CCI_U_TYPE_FLOAT:
    case CCI_U_TYPE_DOUBLE:
        type = QVariant::Double;
        break;
    case CCI_U_TYPE_DATE:
        type = QVariant::Date;
        break;
    case CCI_U_TYPE_TIME:
        type = QVariant::Time;
        break;
    case CCI_U_TYPE_TIMESTAMP:
        type = QVariant::DateTime;
        break;
    case CCI_U_TYPE_BLOB:
    case CCI_U_TYPE_CLOB:
        type = QVariant::ByteArray;
        break;
    default:
        type = QVariant::String;
        break;
    }
    return type;
}

static void qDeallocatePreparedStmt(QCubridResultPrivate *d)
{
    if (d->request > 0)
    {
        if (cci_close_req_handle(d->request) != 0)
        {
            qWarning() << "Unable to free statement:" << d->preparedStmtId;    //qWarning("Unable to free statement: %s", d->preparedStmtId);
        }
    }
    d->preparedStmt.clear();
    d->preparedStmtId.clear();
}

QCubridResult::QCubridResult(const QCubridDriver *db, QCubridDriverPrivate *p)
    : QSqlResult(db)
{
    d = new QCubridResultPrivate(this);
    d->driver = p;
    d->preparedQueriesEnabled = db->hasFeature(QSqlDriver::PreparedQueries);
}

QCubridResult::~QCubridResult()
{
    if (d != 0)
    {
        delete d;
        d = 0;
    }
}

QString QCubridResult::executedQuery() const
{
    return d->preparedStmt;
}

QVariant QCubridResult::handle() const
{
    return qVariantFromValue(d->request);
}

void QCubridResult::cleanup()
{
    if (d != 0)
    {
        if (d->request > 0)
        {
            if (d->queryResult != 0)
            {
                cci_query_result_free(d->queryResult, d->currentSize);
                d->queryResult = 0;
            }

            if (cci_close_req_handle(d->request) != 0)
            {
                qWarning() << "Unable to free statement:" << d->preparedStmtId;    //qWarning("Unable to free statement: %s", d->preparedStmtId);
            }
        }
        else
        {
            d->request = 0;
            setAt(QSql::BeforeFirstRow);
            d->currentSize = -1;
            setActive(false);
        }
    }
}

bool QCubridResult::fetch(int i)
{
    qWarning() << "fetching i : " << i << " - result count : " << d->queryResult->result_count;

    if (!isActive())
    {
        return false;
    }
    if (i < 0)
    {
        setAt(QSql::BeforeFirstRow);
        return false;
    }
    if (i > d->queryResult->result_count - 1)
    {
        setAt(QSql::AfterLastRow);
        return false;
    }

    if (i == 0)
    {
        int res = cci_cursor(d->request, 1, CCI_CURSOR_FIRST, &(d->driver->lastError));
        if (res == CCI_ER_NO_MORE_DATA)
        {
            qWarning() << "Query END!";
        }
        if ((res = cci_fetch(d->request, &(d->driver->lastError))) < 0)
        {
            qWarning() << "Fetch error! (1)";
        }
        setAt(0);
        return true;
    }

    if (at() == i)
    {
        return true;
    }

    int res = cci_cursor(d->request, (i - at()), CCI_CURSOR_CURRENT, &(d->driver->lastError));
    qWarning() << "i (" << i << ") - " << "at() (" << at() << ") = " << (i - at());
    if (res == CCI_ER_NO_MORE_DATA)
    {
        qWarning() << "Query END!";
    }
    if ((res = cci_fetch(d->request, &(d->driver->lastError))) < 0)
    {
        qWarning() << "Fetch error! (2)";
    }
    setAt(i);
    return true;
}

bool QCubridResult::fetchFirst()
{
    qWarning() << "fetchFirst";
    return fetch(0);
}

bool QCubridResult::fetchLast()
{
    qWarning() << "fetchLast";
    return fetch(d->queryResult->result_count - 1);
}

bool QCubridResult::fetchNext()
{
    int i = at();
    if (i < d->queryResult->result_count - 1)
    {
        return fetch(i + 1);
    }
    else
    {
        return false;
    }
}

bool QCubridResult::fetchPrevious()
{
    int i = at();
    if (i > 0)
    {
        return fetch(i - 1);
    }
    else
    {
        return false;
    }
}

QVariant QCubridResult::data(int i)
{
    int currentPos = at();
    if ((currentPos == QSql::BeforeFirstRow) || (currentPos == QSql::AfterLastRow))
    {
        return QVariant();
    }

    if (i >= d->ColumnCount)
    {
        qWarning("QCubridResult::data: column %d out of range", i);
        return QVariant();
    }

    qWarning() << "CCI_GET_RESULT_INFO_TYPE";

    Cubrid_Type ptype = CCI_GET_RESULT_INFO_TYPE(d->columnsInfo, i + 1);
    QVariant::Type type = qDecodeCubridType(ptype);

    qWarning() << "Valor de QVariant::Type: " << type;

    char *val = 0;
    int indicator;

    int res = cci_get_data(d->request, i + 1, CCI_A_TYPE_STR, &val, &indicator);

        qWarning("--> request: %d", d->request);
        qWarning("--> queryResult.result_count: %d", d->queryResult->result_count);
        qWarning("--> i: %d", i);
        qWarning("--> val: %s", val);
        qWarning("--> res: %d",res);
        qWarning("--> indicator: %d",indicator);

        if(res < 0)
        {
            char s[1024];
            cci_get_err_msg(res, s, 1023);
            qWarning() << s;
        }

    // is this null?
    if (indicator == -1)
    {
        return QVariant(type);
    }

    switch (type)
    {
    case QVariant::Bool:
        return QVariant((bool)(val[0] == 't'));
    case QVariant::String:
        return d->driver->isUtf8 ? QString::fromUtf8(val) : QString::fromAscii(val);
    case QVariant::LongLong:
        if (val[0] == '-')
        {
            return QString::fromLatin1(val).toLongLong();
        }
        else
        {
            return QString::fromLatin1(val).toULongLong();
        }
    case QVariant::Int:
        return atoi(val);
    case QVariant::Double:
        if (ptype == CCI_U_TYPE_NUMERIC)
        {
            if (numericalPrecisionPolicy() != QSql::HighPrecision)
            {
                QVariant retval;
                bool convert;
                double dbl = QString::fromAscii(val).toDouble(&convert);
                if (numericalPrecisionPolicy() == QSql::LowPrecisionInt64)
                {
                    retval = (qlonglong)dbl;
                }
                else if (numericalPrecisionPolicy() == QSql::LowPrecisionInt32)
                {
                    retval = (int)dbl;
                }
                else if (numericalPrecisionPolicy() == QSql::LowPrecisionDouble)
                {
                    retval = dbl;
                }
                if (!convert)
                {
                    return QVariant();
                }
                return retval;
            }
            return QString::fromAscii(val);
        }
        return QString::fromAscii(val).toDouble();
    case QVariant::Date:
        if (val[0] == '\0')
        {
            return QVariant(QDate());
        }
        else
        {
#ifndef QT_NO_DATESTRING
            return QVariant(QDate::fromString(QString::fromLatin1(val), Qt::ISODate));
#else
            return QVariant(QString::fromLatin1(val));
#endif
        }
    case QVariant::Time:
    {
        const QString str = QString::fromLatin1(val);
#ifndef QT_NO_DATESTRING
        if (str.isEmpty())
        {
            return QVariant(QTime());
        }
        if (str.at(str.length() - 3) == QLatin1Char('+') || str.at(str.length() - 3) == QLatin1Char('-'))
            // strip the timezone
            // TODO: fix this when timestamp support comes into QDateTime
        {
            return QVariant(QTime::fromString(str.left(str.length() - 3), Qt::ISODate));
        }
        return QVariant(QTime::fromString(str, Qt::ISODate));
#else
        return QVariant(str);
#endif
    }
    case QVariant::DateTime:
    {
        QString dtval = QString::fromLatin1(val);
#ifndef QT_NO_DATESTRING
        if (dtval.length() < 10)
        {
            return QVariant(QDateTime());
        }
        // remove the timezone
        // TODO: fix this when timestamp support comes into QDateTime
        if (dtval.at(dtval.length() - 3) == QLatin1Char('+') || dtval.at(dtval.length() - 3) == QLatin1Char('-'))
        {
            dtval.chop(3);
        }
        // milliseconds are sometimes returned with 2 digits only
        if (dtval.at(dtval.length() - 3).isPunct())
        {
            dtval += QLatin1Char('0');
        }
        if (dtval.isEmpty())
        {
            return QVariant(QDateTime());
        }
        else
        {
            return QVariant(QDateTime::fromString(dtval, Qt::ISODate));
        }
#else
        return QVariant(dtval);
#endif
    }
    case QVariant::ByteArray:
    {
        char *data = val;
        QByteArray ba((const char *)data);
        return QVariant(ba);
    }
    default:
    case QVariant::Invalid:
        qWarning("QCubridResult::data: unknown data type");
    }
    return QVariant();
}

bool QCubridResult::isNull(int field)
{
    char *val;
    int indicator;
    cci_get_data(d->request, field + 1, CCI_A_TYPE_STR, val, &indicator);

    return (indicator == -1);
}

bool QCubridResult::reset(const QString &query)
{
    cleanup();
    if (!driver())
    {
        return false;
    }
    if (!driver()->isOpen() || driver()->isOpenError())
    {
        return false;
    }
    if (!prepare(query))
    {
        return false;
    }
    return exec();
    //return d->processResults();
}

int QCubridResult::size()
{
    if (d->currentSize == -1)
    {
        return 0;
    }
    else
    {
        return d->currentSize;
    }
}

int QCubridResult::numRowsAffected()
{
    return d->affectedRows;
}

QVariant QCubridResult::lastInsertId() const
{
    if (isActive())
    {
        char *id = d->queryResult->oid;
        if (id != 0)
        {
            return QString::fromLatin1(id);
        }
    }
    return QVariant();
}

QSqlRecord QCubridResult::record() const
{
    QSqlRecord info;
    if (!isActive() || !isSelect())
    {
        return info;
    }

    int count = d->ColumnCount;
    for (int i = 0; i < count; ++i)
    {
        QSqlField f;
        if (d->driver->isUtf8)
        {
            f.setName(QString::fromUtf8(CCI_GET_RESULT_INFO_NAME(d->columnsInfo, i + 1)));
        }
        else
        {
            f.setName(QString::fromLocal8Bit(CCI_GET_RESULT_INFO_NAME(d->columnsInfo, i + 1)));
        }
        f.setType(qDecodeCubridType(CCI_GET_RESULT_INFO_TYPE(d->columnsInfo, i)));
        int len = CCI_GET_RESULT_INFO_SCALE(d->columnsInfo, i + 1);
        int precision = CCI_GET_RESULT_INFO_PRECISION(d->columnsInfo, i + 1);
        // swap length and precision if length == -1
        if (len == -1 && precision > -1)
        {
            len = precision - 4;
            precision = -1;
        }
        f.setLength(len);
        f.setPrecision(precision);
        f.setSqlType(CCI_GET_RESULT_INFO_TYPE(d->columnsInfo, i + 1));
        info.append(f);
    }
    return info;
}

static QString qCreateParamString(const QVector<QVariant> boundValues, const QSqlDriver *driver)
{
    if (boundValues.isEmpty())
    {
        return QString();
    }

    QString params;
    QSqlField f;
    for (int i = 0; i < boundValues.count(); ++i)
    {
        const QVariant &val = boundValues.at(i);

        f.setType(val.type());
        if (val.isNull())
        {
            f.clear();
        }
        else
        {
            f.setValue(val);
        }
        if (!params.isNull())
        {
            params.append(QLatin1String(", "));
        }
        params.append(driver->formatValue(f));
    }
    return params;
}

Q_GLOBAL_STATIC(QMutex, qMutex)

QString qMakePreparedStmtId()
{
    qMutex()->lock();
    static unsigned int qPreparedStmtCount = 0;
    QString id = QLatin1String("qcubridpstmt_") + QString::number(++qPreparedStmtCount, 16);
    qMutex()->unlock();
    return id;
}

void QCubridResult::setQuery(const QString &query)
{
    prepare(query);
}

bool QCubridResult::prepare(const QString &query)
{
    if (!d->preparedQueriesEnabled)
    {
        return QSqlResult::prepare(query);
    }
    //QSqlResult::prepare(query);

    cleanup();

    qWarning() << "executing prepare()...";

    if (!d->preparedStmtId.isEmpty())
    {
        qDeallocatePreparedStmt(d);
    }

    const QString stmtId = qMakePreparedStmtId();
    const QString stmt = query;

    QByteArray qba;
    if(d->driver->isUtf8)
    {
        qba = stmt.toUtf8().data();
    }
    else
    {
        qba = stmt.toLocal8Bit().data();
    }

    char *data = qba.data();

    if ((d->request = cci_prepare(d->driver->connection,
                                  data,
                                  CCI_PREPARE_INCLUDE_OID, &(d->driver->lastError))) < 0)
    {
        setLastError(qMakeError(QCoreApplication::translate("QCUBRIDResult",
                                "Unable to prepare statement"), QSqlError::StatementError, d->driver));

        d->request = 0;
        d->preparedStmtId.clear();
        return false;
    }
    else
    {
        qWarning() << "request id: " << d->request <<"query:" << query;
        d->columnsInfo = cci_get_result_info(d->request, &d->commandType, &d->ColumnCount);
    }

    d->preparedStmtId = stmtId;
    d->preparedStmt = query;
    return true;
}

bool QCubridResult::exec()
{
    if (!d->preparedQueriesEnabled)
    {
        return QSqlResult::exec();
    }

    if (d->request == 0)
    {
        prepare(d->preparedStmt);
    }

    qWarning() << "executing exec()...";
    //cleanup();

    int res = 0;

    QString stmt;

    const QString params = qCreateParamString(boundValues(), d->q->driver());
    if (params.isEmpty())
    {
        stmt = QString::fromLatin1("%1").arg(d->preparedStmt);
    }
    else
    {
        stmt = QString::fromLatin1("EXECUTE %1 (%2)").arg(d->preparedStmt).arg(params);
    }


    qWarning() << "STATEMENT: " << stmt << " REQUEST: " << d->request;

    if ((res = cci_execute(d->request, CCI_EXEC_QUERY_ALL, 0, &(d->driver->lastError))) < 0)
    {
        setLastError(qMakeError(QCoreApplication::translate("QCUBRIDResult",
                                "Unable to execute statement"), QSqlError::StatementError, d->driver));
        qWarning() << "Unable to execute statement" << res;
        qWarning() << d->driver->lastError.err_code;
        qWarning() << d->driver->lastError.err_msg;
    }
    else
    {
        d->affectedRows = res;

    }

    if ((res = cci_fetch_size(d->request, 200)) < 0)
    {
        setLastError(qMakeError(QCoreApplication::translate("QCUBRIDResult",
                                "Unable to fetch size"), QSqlError::StatementError, d->driver));
        qWarning() << "Unable to fetch size" << res;
    }


    //qWarning() << "d->affectedRows : " << d->affectedRows;
    qWarning() << "cci_execute_result : " << cci_execute_result(d->request, &(d->queryResult), &(d->driver->lastError));
    qWarning() << "executedQuery : " << executedQuery();

    return d->processResults();
}

//bool QCubridResult::exec(const QString &query)
//{
//    prepare(query);
//    exec();
//}

/////////////////////////////////////////////////////////////////

QCubridDriver::QCubridDriver(QObject *parent): QSqlDriver(parent)
{
    d = new QCubridDriverPrivate();
    setOpen(false);
    setOpenError(false);
}

QCubridDriver::~QCubridDriver()
{
    //    if (d->connection)
    //        PQfinish(d->connection);
    close();

    if (d != 0)
    {
        delete d;
        d = 0;
    }
}

bool QCubridDriver::open(const QString &db, const QString &user, const QString &password, const QString &host, int port, const QString &options)
{
    if (isOpen())
    {
        close();
    }

    qWarning() << "opening connection...";

    if ((d->connection = cci_connect(
                             host.toLocal8Bit().data(),
                             port,
                             db.toLocal8Bit().data(),
                             user.toLocal8Bit().data(),
                             password.toLocal8Bit().data())) < 0)
    {

        QString connErrorMsg = QString();

        switch (d->connection)
        {
        case CCI_ER_NO_MORE_MEMORY:
            connErrorMsg = QCoreApplication::tr("Memory error during connection");
            break;
        case CCI_ER_HOSTNAME:
            connErrorMsg = QCoreApplication::tr("Hostname error during connection");
            break;
        case CCI_ER_CON_HANDLE:
            connErrorMsg = QCoreApplication::tr("Connection handle error during connection");
            break;
        case CCI_ER_DBMS:
            connErrorMsg = QCoreApplication::tr("DBMS error during connection");
            break;
        case CCI_ER_COMMUNICATION:
            connErrorMsg = QCoreApplication::tr("Communication error during connection");
            break;
        case CCI_ER_CONNECT:
            connErrorMsg = QCoreApplication::tr("Connection error");
            break;
        }

        qWarning() << "connection error...";

        setLastError(qMakeError(connErrorMsg, QSqlError::ConnectionError, d));
        return false;
    }
    else
    {
        setOpen(true);
        setOpenError(false);
        //cci_set_autocommit(d->connection, CCI_AUTOCOMMIT_TRUE);
        return true;
    }
}

bool QCubridDriver::isOpen() const
{
    return (d->connection != 0);
}

void QCubridDriver::close()
{
    if (isOpen())
    {
        if (cci_disconnect(d->connection, &d->lastError) != 0)
        {
            qWarning() << tr("Can not disconnect!");
        }
        else
        {
            d->connection = 0;
        }

        setOpen(false);
        setOpenError(false);
    }
}

QSqlResult *QCubridDriver::createResult() const
{
    return new QCubridResult(this, this->d);
}

QStringList QCubridDriver::tables(QSql::TableType type) const
{
    QStringList tl;
    if (!isOpen())
    {
        return tl;
    }
    QSqlQuery t(createResult());
    t.setForwardOnly(true);

    if (type & QSql::Tables)
    {
        t.exec(QLatin1String("SELECT class_name FROM db_class WHERE owner_name <> 'DBA' AND class_type = 'CLASS' ORDER BY class_name"));
    }
    if (type & QSql::Views)
    {
        t.exec(QLatin1String("SELECT class_name FROM db_class WHERE owner_name <> 'DBA' AND class_type = 'VCLASS' ORDER BY class_name"));
    }
    if (type & QSql::SystemTables)
    {
        t.exec(QLatin1String("SELECT class_name FROM db_class WHERE owner_name = 'DBA' AND class_type = 'CLASS' ORDER BY class_name"));
        while (t.next())
        {
            tl.append(t.value(0).toString());
        }
    }

    return tl;
}

QString QCubridDriver::version()
{
    int res;
    char *db_ver;
    if ((res = cci_get_db_version(d->connection, db_ver, sizeof(db_ver))) < 0)
    {
        return QString();
    }
    else
    {
        return QString::fromLocal8Bit(db_ver);
    }
}

bool QCubridDriver::hasFeature(DriverFeature f) const
{
    switch (f)
    {
    case Transactions:
    case QuerySize:
    case LastInsertId:
    case LowPrecisionNumbers:
    case EventNotifications:
        return true;
    case PreparedQueries:
    case PositionalPlaceholders:
        return true;
    case BatchOperations:
    case NamedPlaceholders:
    case SimpleLocking:
    case FinishQuery:
    case MultipleResultSets:
        return true;
    case BLOB:
        return true;
    case Unicode:
        return d->isUtf8;
    }
    return true;
}







QT_END_NAMESPACE





/*
int con = 0, req = 0, col_count = 0, res, ind, i;
    T_CCI_ERROR error;
    T_CCI_COL_INFO *res_col_info;
    T_CCI_SQLX_CMD cmd_type;
    char *buffer, db_ver[16];
    printf("Program started! ");
    if ((con=cci_connect("localhost", 30000, "demodb", "PUBLIC", ""))<0) {
        printf( "%s(%d): cci_connect fail ", __FILE__, __LINE__);
        return -1;
    }

    if ((res=cci_get_db_version(con, db_ver, sizeof(db_ver)))<0) {
        printf( "%s(%d): cci_get_db_version fail ", __FILE__, __LINE__);
        goto handle_error;
    }
    printf("DB Version is %s ",db_ver);
    if ((req=cci_prepare(con, "select * from event", 0,&error))<0) {
        printf( "%s(%d): cci_prepare fail(%d) ", __FILE__, __LINE__,error.err_code);
        goto handle_error;
    }
    */
