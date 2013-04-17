#include "dbsqlite_SqliteDatabaseEngine.h"

#include <QSqlDriver>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include <QMutexLocker>

#include "core_Table.h"
#include "Version.h"
#include "core_Column.h"
#include "Project.h"
#include "core_UserDataType.h"
#include "Index.h"
#include "core_TableInstance.h"
#include "ForeignKey.h"
#include "db_DatabaseBuiltinFunction.h"
#include "core_View.h"
#include "Connection.h"
#include "core_Procedure.h"
#include "core_Function.h"
#include "db_Codepage.h"
#include "core_Trigger.h"
#include "TrueFalseSp.h"
#include "ValueListSp.h"
#include "ValueSp.h"
#include "SpInstance.h"
#include "core_UserDataType.h"
#include "SqliteConnection.h"

QVector<DatabaseBuiltinFunction>* SqliteDatabaseEngine::s_builtinFunctions = 0;
QVector<Sp*>* SqliteDatabaseEngine::s_sqliteSpecificProperties = 0;
int SqliteDatabaseEngine::m_connectionCounter = 1;
QMutex* SqliteDatabaseEngine::m_connectionMutex = 0;
SqliteDatabaseEngine* SqliteDatabaseEngine::s_instance = 0;

SqliteDatabaseEngine* SqliteDatabaseEngine::instance()
{
    if(s_instance == 0)
    {
        s_instance = new SqliteDatabaseEngine();
    }

    return s_instance;
}

QUuid SqliteDatabaseEngine::getClassUid() const
{
    return QUuid(m_classUid);
}

SqliteDatabaseEngine::SqliteDatabaseEngine() : DefaultDatabaseEngine("Sqlite", uidSqliteDb), m_revEngMappings(), m_oneTimeMappings()
{
    static QVector<DatabaseBuiltinFunction> v = buildFunctions();
    static QVector<Sp*> t = buildSps();
    s_sqliteSpecificProperties = &t;
    s_builtinFunctions = &v;
    m_connectionMutex = new QMutex();
}

SqliteDatabaseEngine::~SqliteDatabaseEngine()
{
}

// this should be thread safe
QString SqliteDatabaseEngine::provideConnectionName(const QString& prefix)
{
    QMutexLocker lock(m_connectionMutex);
    QString t = prefix + QString::number(m_connectionCounter++);
    lock.unlock();
    return t;
}

bool SqliteDatabaseEngine::reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, const QStringList& procs,
                                                  const QStringList& funcs, const QStringList& triggers, Project*p, bool relaxed)
{
    try
    {
    Version* v = p->getWorkingVersion();
    m_revEngMappings.clear();
    m_oneTimeMappings.clear();

    for(int i=0; i<tables.size(); i++)
    {
        Table* tab = reverseEngineerTable(c, tables.at(i), p, relaxed, v);
        if(tab) v->addTable(tab, true); // TODO: is this true that this is true?
    }

    for(int i=0; i<views.size(); i++)
    {
        View* view = reverseEngineerView(c, views.at(i), v);
        if(view) v->addView(view, true);
    }

    for(int i=0; i<triggers.size(); i++)
    {
        Trigger* t= reverseEngineerTrigger(c, triggers.at(i), v);
        if(t) v->addTrigger(t, true);
    }

    // now populate the foreign keys
    {
#if 0
        QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

        bool ok = dbo.isOpen();

        if(ok)
        {
            QString s = "SELECT distinct CONCAT( table_name, '.', column_name, ':', referenced_table_name, '.', referenced_column_name ) AS list_of_fks, constraint_name "
                    "FROM information_schema.KEY_COLUMN_USAGE WHERE REFERENCED_TABLE_SCHEMA = '"
                    + c->getDb() +
                    "' AND REFERENCED_TABLE_NAME is not null ORDER BY TABLE_NAME, COLUMN_NAME";
            QSqlQuery query(dbo);
            query.exec(s);
            bool foundAtLeastOneForeignKey = false;
            while(query.next())
            {
                QString val = query.value(0).toString();
                QString name = query.value(1).toString();
                QString referencee = val.left(val.indexOf(':'));
                QString referenced = val.mid(val.indexOf(':') + 1);

                QString referenceeTableName = referencee.left(referencee.indexOf('.'));
                QString referenceeColumnName = referencee.mid(referencee.indexOf('.') + 1);

                QString referencedTableName = referenced.left(referenced.indexOf('.'));
                QString referencedColumnName = referenced.mid(referenced.indexOf('.') + 1);

                Table* referenceeTable = v->getTable(referenceeTableName);
                Table* referencedTable = v->getTable(referencedTableName);
                Column* referenceeColumn = referenceeTable->getColumn(referenceeColumnName);
                Column* referencedColumn = referencedTable->getColumn(referencedColumnName);
                // now we should check that the columns have the same data type ...
                if(referencedColumn->getDataType() != referenceeColumn->getDataType())
                {
                    // TODO initially here was nothing ...
                    referencedColumn->setDataType(referenceeColumn->getDataType());
                }

                ForeignKey::ColumnAssociation* fkAssociation = new ForeignKey::ColumnAssociation(referencedTable, referencedColumn, referenceeTable, referenceeColumn);
                ForeignKey* fk = new ForeignKey(v, QUuid::createUuid().toString());
                fk->setName(name);
                fk->addAssociation(fkAssociation);
                referenceeTable->addForeignKey(fk);
                foundAtLeastOneForeignKey = true;
            }

            if(!foundAtLeastOneForeignKey)
            {
                v->setSpecialValidationFlags(1);
            }

            dbo.close();
        }
#endif
    }
    }
    catch(...)
    {
        qDebug() << "exception";
        return false;
    }

    return true;
}

QSqlDatabase SqliteDatabaseEngine::getQSqlDatabaseForConnection(Connection *conn)
{

    SqliteConnection* c = dynamic_cast<SqliteConnection*>(conn);
    if(!c)
    {
        return QSqlDatabase::database();
    }

    QString newConnName = provideConnectionName("getConnection");
    QString dbType = "QSQLITE2";
    if(c->getVersion() == 3) dbType = "QSQLITE";
    QSqlDatabase dbo = QSqlDatabase::addDatabase(dbType, newConnName);

    dbo.setDatabaseName(c->getFileName());

    dbo.open();

    if(!dbo.isOpen())
    {
        qDebug() << dbo.lastError();
    }

    return dbo;
}

QStringList SqliteDatabaseEngine::getAvailableViews(Connection* conn)
{
    SqliteConnection* c = dynamic_cast<SqliteConnection*>(conn);
    if(!c)
    {
        return QStringList();
    }
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

    bool ok = dbo.isOpen();
    QStringList result;

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot get views"), dbo.lastError());
        return result;
    }

    QSqlQuery query (dbo);

    query.exec("select name from sqlite_master where type = 'view'");

    while(query.next())
    {
        QString tab = query.value(0).toString();
        result.append(tab);
    }
    dbo.close();
    return result;
}

QStringList SqliteDatabaseEngine::getAvailableTriggers(Connection* c)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

    bool ok = dbo.isOpen();
    QStringList result;

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot get triggers"), dbo.lastError());
        return result;
    }

    QSqlQuery query (dbo);

    query.exec("show triggers");

    while(query.next())
    {
        QString tab = query.value(0).toString();
        result.append(tab);
    }
    dbo.close();
    return result;
}


QStringList SqliteDatabaseEngine::getAvailableStoredProcedures(Connection* /*c*/)
{
     return QStringList();
}

QStringList SqliteDatabaseEngine::getAvailableStoredFunctions(Connection* /*c*/)
{
    return QStringList();
}

QStringList SqliteDatabaseEngine::getAvailableTables(Connection* c)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

    bool ok = dbo.isOpen();
    QStringList result;

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot get tables"),dbo.lastError());
        return result;
    }

    QSqlQuery query(dbo);



    ok = query.exec("select name from sqlite_master where type = 'table';");
    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot get tables") + QString(" <<< ") + dbo.databaseName() + ">>>", query.lastError() );
        qDebug() << lastError;
        return result;
    }

    while(query.next())
    {
        QString tab = query.value(0).toString();
        result.append(tab);
    }
    dbo.close();
    return result;
}

Procedure* SqliteDatabaseEngine::reverseEngineerProc(Connection */*c*/, const QString &/*procName*/, Version */*v*/)
{
    return 0;
}

Function* SqliteDatabaseEngine::reverseEngineerFunc(Connection */*c*/, const QString &/*funcName*/, Version */*v*/)
{
    return 0;
}

View* SqliteDatabaseEngine::reverseEngineerView(Connection* c, const QString& viewName, Version *v)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);


    bool ok = dbo.open();

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot reverse engineer a view"), dbo.lastError());
        return 0;
    }

    View* view = 0;
    QSqlQuery query(dbo);
    QString t = QString("select sql from from sqlite_master where type='view' and name='") + viewName + "'";
    query.exec(t);
    while(query.next())
    {
        QString sql = query.value(1).toString();
        view = new View(true, QUuid::createUuid().toString(), v);
        view->setSql(sql);
    }
    dbo.close();
    return view;
}

QStringList SqliteDatabaseEngine::getColumnsOfTable(Connection *c, const QString &tableName)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    QStringList result;

    bool ok = db.isOpen();

    if(!ok)
    {
        return result;
    }

    QSqlQuery query(db);
    query.exec(QString("pragma table_info(") + tableName + ")");

    int fieldNo = query.record().indexOf("name");

    while(query.next())
    {
        QString field_name = query.value(fieldNo).toString();
        result.append(field_name);
    }
    return result;
}

Table* SqliteDatabaseEngine::reverseEngineerTable(Connection *c, const QString& tableName, Project* p, bool relaxed, Version *ver)
{
    QSqlDatabase dbo = c->getQSqlDatabase();
    Version* v = p->getWorkingVersion();
    bool ok = dbo.isOpen();

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot reverse engineer a table"), dbo.lastError());
        return 0;
    }

    Table* tab = new Table(v, QUuid::createUuid().toString());
    tab->initializeFor(this, QUuid(uidTable));
    tab->setName(tableName);

    // fetch all the columns of the table
    {
    QSqlQuery query(dbo);
    query.exec("pragma table_info(" + tableName + ")");

    int fieldNo = query.record().indexOf("name");
    int typeNo = query.record().indexOf("type");
    int nulldNo = query.record().indexOf("notnull");
    int keyNo = query.record().indexOf("pk");
    int defNo = query.record().indexOf("dflt_value");

    while(query.next())
    {
        QString field_name = query.value(fieldNo).toString();
        QString type = query.value(typeNo).toString().toUpper();
        QString nullable = query.value(nulldNo).toString();
        QString keyness = query.value(keyNo).toString();
        QString defaultValue = query.value(defNo).toString();

        UserDataType* udt = 0;
        QString oneTimeKey = field_name + type + nullable + defaultValue;
        bool found = false;
        if(m_oneTimeMappings.contains(oneTimeKey))
        {
            found = true;
            udt = m_oneTimeMappings.value(oneTimeKey);
        }
        else
        {
            udt = v->provideDatatypeForSqlType(field_name, type, nullable, defaultValue, relaxed);
            m_oneTimeMappings.insert(oneTimeKey, udt);
        }

        // hm ... PK check?
        Column* col = new Column(QUuid::createUuid().toString(), field_name, udt, QString::compare(keyness, "0", Qt::CaseInsensitive) == 0, ver);

        // and add the column to the table
        if(!found) m_revEngMappings.insert(udt, col);
        tab->addColumn(col);
    }
    }

    // now populate the indices of the table
    {

    QSqlQuery query(dbo);
    query.exec("select * from sqlite_master where type='index' and tbl_name='" + tableName + "'");

    int idxIndexName = query.record().indexOf("name");

    QMap<QString, Index*> createdIndexes;

    while(query.next())
    {
        QString indexName = query.value(idxIndexName).toString();

        // now find the column(s) of the index
        QSqlQuery query2(dbo);
        query2.exec(QString("pragma index_info(") + idxIndexName + ")");

        int idxColName = query2.record().indexOf("name");
        int idxSeq = query2.record().indexOf("seqno");

        QString columnname = query2.value(idxColName).toString();
        QString seqinindex= query2.value(idxSeq).toString();

        Index* idx = 0;
        QString order = "";
        if(createdIndexes.keys().contains(indexName))
        {
            idx = createdIndexes[indexName];
        }
        else
        {
            idx = new Index(indexName, tab, QUuid::createUuid().toString(), ver);
            createdIndexes.insert(indexName, idx);
        }

        idx->addColumn(tab->getColumn(columnname), order, seqinindex.toInt());
        if(!tab->hasIndex(indexName))
        {
            tab->addIndex(idx);
        }

    }
    }

    // populate the startup values
    {
        QString s = "select ";
        for(int i=0; i<tab->getColumnCount(); i++)
        {
            s += tab->getColumns().at(i)->getName();
            if(i<tab->getColumnCount() -1) s+=", ";
        }
        s += " from ";
        s += tab->getName();
        QSqlQuery query (dbo);
        query.exec(s);
        QVector <QVector <QString> > defaultValues;
        while(query.next())
        {
            QVector <QString> row;
            for(int i=0; i<tab->getColumnCount(); i++)
            {
                QVariant atI = query.value(i);
                if(atI.isNull())
                {
                    row.append("NULL");
                }
                else
                {
                    row.append(atI.toString());
                }
            }

            defaultValues.append(row);
        }

        if(! p->oopProject())
        {
            tab->setDefaultValues(defaultValues);
        }
        else
        {
            // convert the vector above to the required hash
            QHash < QString, QVector<QString> > values;
            for(int i=0; i<defaultValues.size(); i++)
            {
                QVector<QString> defVsI = defaultValues.at(i);
                for(int j=0; j<defVsI.size(); j++)
                {
                    if(values.keys().contains(tab->getColumns().at(j)->getName()))
                    {
                        values[tab->getColumns().at(j)->getName()].append(defVsI.at(j));
                    }
                    else
                    {
                        QVector<QString> newValues;
                        newValues.append(defVsI.at(j));
                        values[tab->getColumns().at(j)->getName()] = newValues;
                    }
                }
            }
            // and create a table instance for it
            TableInstance* inst = v->instantiateTable(tab, false);
            QString instName = NameGenerator::getUniqueName(v, (itemGetter)&Version::getTableInstance, tab->getName());
            inst->setName(instName);
            inst->setValues(values);
        }
    }

    dbo.close();
    return tab;
}

bool SqliteDatabaseEngine::executeSql(Connection* c, const QStringList& sqls, const QStringList &uid, QString& lastSql, bool rollbackOnError)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    bool ok = db.isOpen();
    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot connect to:") + c->getFullLocation(), db.lastError());
        return false;
    }

    bool transactionSucces = db.transaction();

    for(int i=0; i<sqls.size(); i++)
    {
        lastSql = sqls[i].trimmed();
        if(lastSql.length() > 0)
        {
            QSqlQuery query(db);

//            qDebug() << lastSql  << uid;
            if(!query.exec(lastSql))
            {
                lastError = formatLastError(QObject::tr("Cannot run a query"), query.lastError());
                QString theUid = nullUid;
                if(i < uid.size() - 1) theUid = uid[i];
                lastError += "<!-- UID:" + theUid + "<!-- /UID --> <br><br><pre>" + lastSql;
                if(transactionSucces && rollbackOnError)
                {
                    db.rollback();
                }

//                qDebug() << " <-- ERROR" << lastError ;
                db.close();
                return false;
            }
//            qDebug() << " <-- OK";
        }
    }

    if(transactionSucces)
    {
        db.commit();
    }
    db.close();
    return true;
}

QString SqliteDatabaseEngine::getDefaultDatatypesLocation()
{
    return "mysql.defaults";
}

// poor guy, canot return anyting
QStringList SqliteDatabaseEngine::getAvailableDatabases(const QString& /*host*/, const QString& /*user*/, const QString& /*pass*/, int /*port*/)
{
    return QStringList();
}

// Sqlite cannot drop a database
bool SqliteDatabaseEngine::dropDatabase(Connection* /*c*/)
{
    return true;
}

QString SqliteDatabaseEngine::formatLastError(const QString& header, const QSqlError &error)
{
    QString errorText = "<b>" + header + "</b>";
    errorText += "<br><br>";
    errorText += "<b>"+ QObject::tr("DB Error: ")  + "</b><!-- DBE: -->"+ error.databaseText() + "<!-- /DBE --><br>";
    errorText += "<b>" + QObject::tr("Driver Error: ")  + "</b><!-- DRV: -->"+ error.driverText() + "<!-- /DRV --><br>";
    errorText += "<b>" + QObject::tr("Error Number: ")  + "</b><!-- NR: -->"+  QString::number(error.number()) + "<!-- /NR --><br>";
    errorText += "<b>" + QObject::tr("Error Type: ") + "</b>";
    switch(error.type())
    {
    case QSqlError::NoError: errorText += "<!-- TYPE: -->No Error<!-- /TYPE -->";
        break;
    case QSqlError::ConnectionError: errorText += "<!-- TYPE: -->Connection Error<!-- /TYPE -->";
        break;
    case QSqlError::StatementError: errorText += "<!-- TYPE: -->Statement Error<!-- /TYPE -->";
        break;
    case QSqlError::TransactionError: errorText += "<!-- TYPE: -->Transaction Error<!-- /TYPE -->";
        break;
    case QSqlError::UnknownError: errorText += "<!-- TYPE: -->Unknown Error<!-- /TYPE -->";
        break;
    }
    errorText += "<br>";
    return errorText;
}

// cannot create database
bool SqliteDatabaseEngine::createDatabase(Connection* /*c*/)
{
    return true;
}

QVector<DatabaseBuiltinFunction> SqliteDatabaseEngine::getBuiltinFunctions()
{
    return *s_builtinFunctions;
}


QVector<DatabaseBuiltinFunction> SqliteDatabaseEngine::buildFunctions()
{
    static QVector<DatabaseBuiltinFunction> result;
    QString X = QString("X");


#define RET_NUMERIC     UserDataType("return", DT_NUMERIC, nullUid, 0)
#define RET_DATETIME    UserDataType("return", DT_DATETIME, nullUid, 0)
#define RET_STRING      UserDataType("return", DT_STRING, nullUid, 0)

#define PAR_STRING      DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_STRING, nullUid, 0), true)
#define PAR_NUMERIC     DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_NUMERIC, nullUid, 0), true)
#define PAR_VARIABLE    DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_VARIABLE, nullUid, 0), true)

#define OPAR_STRING      DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_STRING, nullUid, 0), false)
#define OPAR_NUMERIC     DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_NUMERIC, nullUid, 0), false)

#define FUNC result.append(DatabaseBuiltinFunction(QString

    FUNC("@if"),           FT_CONTROLFLOW,  RET_STRING, PAR_STRING, PAR_STRING, PAR_STRING, "IF(expr1,expr2,expr3) If expr1 is TRUE (expr1 <> 0 and expr1 <> NULL) then IF() returns expr2; otherwise it returns expr3. IF() returns a numeric or string value, depending on the context in which it is used."));

    FUNC("@abs"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ABS(X) Returns the absolute value of X"));

    FUNC("@date"),         FT_DATETIME,  RET_STRING, PAR_STRING, "DATE(expr) Extracts the date part of the date or datetime expression expr."));
    FUNC("@time"),         FT_DATETIME,  RET_NUMERIC, PAR_STRING, "TIME(expr) Extracts the time part of the time or datetime expression expr and returns it as a string."));

    FUNC("@avg"),          FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "AVG(expr) Returns the average value of expr."));
    FUNC("@count"),        FT_AGGREGATE, RET_NUMERIC, PAR_VARIABLE, "COUNT([DISTINCT]expr) Returns a count of the number of non-NULL values of expr in the rows retrieved by a SELECT statement. The result is a BIGINT value."));
    FUNC("@group_concat"), FT_AGGREGATE, RET_STRING,  PAR_VARIABLE, "GROUP_CONCAT(expr) Returns a string result with the concatenated non-NULL values from a group. It returns NULL if there are no non-NULL values. "));
    FUNC("@max"),          FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "MAX(expr) Returns the max value of expr."));
    FUNC("@min"),          FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "MIN(expr) Returns the min value of expr."));

    FUNC("@char"),         FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_VARIABLE,  "CHAR(N,...) interprets each argument N as an integer and returns a string consisting of the characters given by the code values of those integers. NULL values are skipped."));
    FUNC("@upper"),        FT_STRING,   RET_STRING,   PAR_STRING, "UPPER(str) Returns the string str with all characters changed to uppercase according to the current character set mapping."));

    return result;
}

const DatabaseBuiltinFunction& SqliteDatabaseEngine::getBuiltinFunction(const QString& name)
{
    static DatabaseBuiltinFunction no_function;
    for(int i=0; i<(*s_builtinFunctions).size(); i++)
    {
        if((*s_builtinFunctions).at(i).getName().mid(1).toUpper() == name.toUpper())   // remove the leading @
        {
            return (*s_builtinFunctions).at(i);
        }
    }
    return no_function;
}

bool SqliteDatabaseEngine::tryConnect(Connection* c)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);
    bool ok = dbo.isOpen();

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot connect to the database"), dbo.lastError());
        return false;
    }
    dbo.close();
    return true;

}


QString SqliteDatabaseEngine::getDelimiterKeyword()
{
    return "delimiter";
}

QVector<Codepage*> SqliteDatabaseEngine::getCodepages()
{
    QStringList codepages;
    codepages << "--Unicode - UCS2"  <<
                 "ucs2_bin"<<
                 "ucs2_czech_ci"<<
                 "ucs2_danish_ci"<<
                 "ucs2_esperanto_ci"<<
                 "ucs2_estonia_ci"<<
                 "ucs2_general_ci"<<
                 "ucs2_hungarian_ci"<<
                 "ucs2_icelandic_ci"<<
                 "ucs2_latvian_ci"<<
                 "ucs2_lithuanian_ci"<<
                 "ucs2_persian_ci"<<
                 "ucs2_polish_ci"<<
                 "ucs2_roman_ci"<<
                 "ucs2_romanian_ci"<<
                 "ucs2_slovak_ci"<<
                 "ucs2_slovenian_ci"<<
                 "ucs2_spanish2_ci"<<
                 "ucs2_spanish_ci"<<
                 "ucs2_swedish_ci"<<
                 "ucs2_turkish_ci"<<
                 "ucs2_unicode_ci"<<
                 "--Unicode - UTF8"<<
                 "utf8_bin"<<
                 "utf8_czech_ci"<<
                 "utf8_danish_ci"<<
                 "utf8_esperanto_ci"<<
                 "utf8_estonia_ci"<<
                 "utf8_general_ci"<<
                 "utf8_hungarian_ci"<<
                 "utf8_icelandic_ci"<<
                 "utf8_latvian_ci"<<
                 "utf8_lithuanian_ci"<<
                 "utf8_persian_ci"<<
                 "utf8_polish_ci"<<
                 "utf8_roman_ci"<<
                 "utf8_romanian_ci"<<
                 "utf8_slovak_ci"<<
                 "utf8_slovenian_ci"<<
                 "utf8_spanish2_ci"<<
                 "utf8_spanish_ci"<<
                 "utf8_swedish_ci"<<
                 "utf8_turkish_ci"<<
                 "utf8_unicode_ci"<<
                 "--Western European"<<
                 "ascii_bin"<<
                 "ascii_general"<<
                 "dec8_bin"<<
                 "dec8_swedish_ci "<<
                 "hp8_bin"<<
                 "hp8_english_ci "<<
                 "latin1_bin"<<
                 "latin1_danish_ci"<<
                 "latin1_general_ci"<<
                 "latin1_general_cs" <<
                 "latin1_german1_ci" <<
                 "latin1_german2_ci" <<
                 "latin1_spanish_ci" <<
                 "latin1_swedish_ci"<<
                 "--Central European"<<
                 "cp1250_bin"<<
                 "cp1250_croatian_ci" <<
                 "cp1250_czech_cs" <<
                 "cp1250_general_ci" <<
                 "cp852_bin"<<
                 "cp852_general_ci" <<
                 "keybcs2_bin"<<
                 "keybcs2_general_ci" <<
                 "latin2_bin"<<
                 "latin2_croatian_ci" <<
                 "latin2_czech_cs" <<
                 "latin2_general_ci"<<
                 "latin2_hungarian_ci"<<
                 "macce_bin"<<
                 "macce_general_ci" <<
                 "--South European"<<
                 "armscii8_bin"<<
                 "armscii8_general_ci" <<
                 "cp1256_bin"<<
                 "cp1256_general_ci" <<
                 "geostd8_bin"<<
                 "geostd8_general_ci" <<
                 "greek_bin"<<
                 "greek_general_ci" <<
                 "hebrew_bin"<<
                 "hebrew_general_ci" <<
                 "latin5_bin"<<
                 "latin5_turkish_ci"<<
                 "--Baltic"<<
                 "cp1257_bin"<<
                 "cp1257_general_ci"<<
                 "cp1257_lithuanian_ci"<<
                 "latin7_bin"<<
                 "latin7_estonia_cs" <<
                 "latin7_general_ci"<<
                 "latin7_general_cs"<<
                 "--Cyrillic"<<
                 "cp1251_bin"<<
                 "cp1251_bulgarian_ci"<<
                 "cp1251_general_ci"<<
                 "cp1251_general_cs"<<
                 "cp1251_ukrainian_ci"<<
                 "cp866_bin"<<
                 "cp866_general_ci" <<
                 "koi8r_bin"<<
                 "koi8r_general_ci" <<
                 "koi8u_bin"<<
                 "koi8u_general_ci" <<
                 "--Asian"<<
                 "big5_bin"<<
                 "big5_chinese_ci" <<
                 "cp932_bin"<<
                 "cp932_japanese_ci" <<
                 "eucjpms_bin"<<
                 "eucjpms_japanese_ci" <<
                 "euckr_bin"<<
                 "euckr_korean_ci" <<
                 "gb2312_bin"<<
                 "gb2312_chinese_ci"<<
                 "gbk_bin"<<
                 "gbk_chinese_ci" <<
                 "sjis_bin"<<
                 "sjis_japanese_ci" <<
                 "tis620_bin"<<
                 "tis620_thai_ci" <<
                 "ujis_bin"<<
                 "ujis_japanese_ci" ;

    QVector<Codepage*> result;
    for(int i=0; i<codepages.size(); i++)
    {
         Codepage* cp = new Codepage(codepages.at(i));
         result.append(cp);
    }

    return result;
}

QStringList SqliteDatabaseEngine::getTriggerEvents()
{
    QStringList result;
    result << "INSERT" << "UPDATE" << "DELETE";
    return result;
}

QStringList SqliteDatabaseEngine::getTriggerTimings()
{
    QStringList result;
    result << "BEFORE" << "AFTER";
    return result;
}

Trigger* SqliteDatabaseEngine::reverseEngineerTrigger(Connection *c, const QString& procName, Version *v)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

    bool ok = dbo.isOpen();

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot reverse engineer trigger"), dbo.lastError());
        return 0;
    }

    QSqlQuery query (dbo);

    query.exec("show triggers");

    Trigger *result = 0;
    while(query.next())
    {
        QString trigName = query.value(0).toString();
        if(trigName == procName)
        {
            QString event = query.value(1).toString();
            QString table =  query.value(2).toString();
            QString stmt =  query.value(3).toString();
            QString timing =  query.value(4).toString();
            result = new Trigger(trigName, QUuid::createUuid().toString(), v);
            result->setEvent(event);
            result->setTable(table);
            result->setSql(stmt);
            result->setTime(timing);
        }
    }
    dbo.close();
    return result;
}

QString SqliteDatabaseEngine::getTableDescriptionScript(const QString& tabName)
{
    QString result = QString("pragma table_info(") + tabName + ")";
    return result;
}

QStringList SqliteDatabaseEngine::getAvailableIndexes(Connection* c)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    QStringList result;

    bool ok = db.isOpen();

    if(!ok)
    {
        return result;
    }

    QSqlQuery query(db);
    query.exec("SELECT * FROM SQLITE_MASTER WHERE type='index'");

    int tabNameIdx = query.record().indexOf("tbl_name");
    int indexNameIdx = query.record().indexOf("name");

    while(query.next())
    {
        QString tabName = query.value(tabNameIdx).toString();
        QString indexName = query.value(indexNameIdx).toString();

        result.append(tabName + "." + indexName);
    }
    db.close();
    return result;
}

QString SqliteDatabaseEngine::getTableCreationScript(Connection* c, const QString& tabName)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    QString result;

    bool ok = db.isOpen();

    if(!ok)
    {
        return result;
    }


    QSqlQuery query(db);
    query.exec(QString("select sql from sqlite_master where type='table' and name='") + tabName + "'");

    while(query.next())
    {
        result = query.value(0).toString();
    }
    db.close();
    return result;
}

// TODO: find a way to not to create this vector all the time
QVector<Sp*> SqliteDatabaseEngine::getDatabaseSpecificProperties() const
{
    return *s_sqliteSpecificProperties;
}

QVector<Sp*> SqliteDatabaseEngine::buildSps()
{
    QVector<Sp*> result;
    return result;
}

Sp* SqliteDatabaseEngine::getSpForSqlRole(const QString& uid) const
{
    const QVector<Sp*>& allsps = getDatabaseSpecificProperties();
    for(int i=0; i<allsps.size(); i++)
    {
        if(allsps.at(i)->getSqlRoleUid() == uid)
        {
            return allsps.at(i);
        }
    }
    return 0;
}

bool SqliteDatabaseEngine::tableBlocksForeignKeyFunctionality(const Table*) const
{
    // SQLITE is not as picky as MySQL
    return false;
}

bool SqliteDatabaseEngine::injectMetadata(Connection *c, const Version *v)
{
    QDomDocument doc("DBM");
    QDomElement root = doc.createElement("Metadata");
    v->serialize(doc, root);

    doc.appendChild(root);
    QString xml = doc.toString();
//    qDebug() << xml;

    QStringList sqls;
    sqls << "DROP TABLE IF EXISTS DDM_META";
    sqls << "create table DDM_META                                          \
            (                                                               \
                INJECT_TIME timestamp,                                      \
                IDX integer(10) primary key auto_increment ,                \
                METADATA_CHUNK text                                         \
            )";

    QString last;
    bool b = executeSql(c, sqls, QStringList(), last, false);
    if(!b) return false;

    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    if(!db.isOpen()) return false;
    QSqlQuery q(db);
    q.prepare("INSERT INTO DDM_META(inject_time, metadata_chunk)  VALUES(SYSDATE(), :md)");
    QString hexedXml = toHexString(xml);
    QStringList chopped = chopUpString(hexedXml, 200);
    for(int i=0; i<chopped.size(); i++)
    {
        q.bindValue(":md", chopped.at(i));
        if(!q.exec())
        {
            lastError = formatLastError(QObject::tr("Cannot inject metadata"), db.lastError());
//            qDebug() << lastError;
            return false;
        }
    }
    return true;
}

// TODO: this should go to a common place
QString SqliteDatabaseEngine::getDbMetadata(Connection *c)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    if(!db.isOpen()) return "";
    {
    QSqlQuery q(db);
    QString g = "SELECT count(*) FROM sqlite_master WHERE name='DDM_META' and type='table'";
    if(!q.exec(g))
    {
        return "";
    }

    bool foundMetaTable = false;
    while(q.next())
    {
        int cnt = q.value(0).toInt();
        if(cnt == 1)
        {
            foundMetaTable = true;
        }
    }
    if(!foundMetaTable) return "";
    }

    QString dq = "SELECT metadata_chunk FROM DDM_META ORDER BY IDX";
    QSqlQuery q(db);
    if(!q.exec(dq))
    {
        return "";
    }

    QString xmd = "";
    QString last;
    bool f = true;
    while(q.next())
    {
        last = "";
        QString chunk = q.value(0).toString();
        for(int i=0; i<chunk.length();)
        {
            QString hex = chunk.at(i);
            hex += chunk.at(i+1);
            i += 2;
            xmd += QChar((char)(hex.toInt(0, 16)));
            last+= QChar((char)(hex.toInt(0, 16)));
        }
        if(f)
        {
//            qDebug() << xmd;
            f = false;
        }

    }
//    qDebug() << last;
    return xmd;
}
