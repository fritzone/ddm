/****************************************************************************
**
** Copyright (C) 2012 Antonio Marcio Adiodato de Menezes.
** All rights reserved.
** Contact: Antonio Menezes (antoniomenezes@gmail.com)
**
** This file is part of the CUBRID Qt Sql Driver.
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

#ifndef QSQL_CUBRID_H
#define QSQL_CUBRID_H

#include <QtSql/qsqlresult.h>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlrecord.h>
#include <qdebug.h>
#include <qstringlist.h>
#include <qstring.h>
#include <cas_cci.h>
#include <cas_error.h>

#ifdef QT_PLUGIN
#define Q_EXPORT_SQLDRIVER_CUBRID
#else
#define Q_EXPORT_SQLDRIVER_CUBRID Q_SQL_EXPORT
#endif

QT_BEGIN_HEADER

typedef int Cubrid_Conn;
typedef int Cubrid_Request;
typedef T_CCI_QUERY_RESULT Cubrid_Query_Result;
typedef T_CCI_COL_INFO Cubrid_Columns_Info;
typedef T_CCI_SQLX_CMD Cubrid_Command_Type;
typedef T_CCI_ERROR Cubrid_Error;
typedef T_CCI_U_TYPE Cubrid_Type;

QT_BEGIN_NAMESPACE

class QCubridResultPrivate;
class QCubridDriverPrivate;
class QCubridDriver;
class QSqlRecordInfo;

class Q_EXPORT_SQLDRIVER_CUBRID QCubridResult : public QSqlResult //QSqlCachedResult
{
    friend class QCubridResultPrivate;
public:    
    QCubridResult(const QCubridDriver* db, QCubridDriverPrivate* p);
    ~QCubridResult();

    QVariant handle() const;
    //void virtual_hook(int id, void *data);

protected:
    void cleanup();
    bool fetch(int i);
    bool fetchFirst();
    bool fetchLast();
    bool fetchNext();
    bool fetchPrevious();
    QVariant data(int i);
    bool isNull(int field);
    QString executedQuery () const;

//    bool fetch(int /* index */) { return false; }
//    bool fetchFirst() { return false; }
//    bool fetchLast() { return false; }
//    QVariant data(int /* index */) { return QVariant(); }
//    bool isNull(int /* index */) { return false; }
    bool reset(const QString &query);


    int size();
    int numRowsAffected();
    QSqlRecord record() const;
    QVariant lastInsertId() const;
    bool prepare(const QString& query);
    bool exec();
    void setQuery ( const QString & query );

private:
    QCubridResultPrivate *d;
};

class Q_EXPORT_SQLDRIVER_CUBRID QCubridDriver : public QSqlDriver
{
    Q_OBJECT
    //friend class QCubridDriverPrivate;
    //friend class QCubridResultPrivate;
public:
    explicit QCubridDriver(QObject *parent = 0);
    ~QCubridDriver();
    
    bool hasFeature(DriverFeature /* feature */) const;
    bool open(const QString &db, const QString &user, const QString &password, const QString &host, int port, const QString &options);
    bool isOpen() const;
    void close();
    QSqlResult *createResult() const;
    QStringList tables(QSql::TableType) const;

private:
    QCubridDriverPrivate *d;
    QString version();
};

QT_END_NAMESPACE

QT_END_HEADER
#endif // QSQL_CUBRID_H
