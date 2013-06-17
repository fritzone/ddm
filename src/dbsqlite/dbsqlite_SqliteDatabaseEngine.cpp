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
int SqliteDatabaseEngine::m_sqliteConnectionCounter = 1;
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
    QString t = prefix + QString::number(m_sqliteConnectionCounter++);
    lock.unlock();
    return t;
}


bool SqliteDatabaseEngine::reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, const QStringList& /*procs*/,
                                                  const QStringList& /*funcs*/, const QStringList& triggers, Project*p, bool relaxed)
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

    query.exec("select name from SQLite_Master where Type = 'trigger'");

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
        QString nullable = query.value(nulldNo).toString(); // in sqlite this is actuall NOT NULL!!!
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
            bool bnullable = QString::compare(nullable, "0", Qt::CaseInsensitive) == 0;
            udt = v->provideDatatypeForSqlType(field_name, type, bnullable, defaultValue, relaxed);
            m_oneTimeMappings.insert(oneTimeKey, udt);
        }

        // hm ... PK check?
        bool pk = keyness == "1";
        Column* col = new Column(QUuid::createUuid().toString(), field_name, udt, pk, ver);

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
        QString s = QString("pragma index_info(") + indexName + ")";
        if(!query2.exec(s))
        {
            return 0;
        }


        QSqlRecord rec = query2.record();

        if(!query2.next())
        {
            continue;
        }
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

        idx->addColumn(tab->getColumn(columnname), order, seqinindex.toInt() + 1);
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
            QString instName = tab->getName(); // no need to get an instance name for it
            inst->setName(instName);
            inst->setValues(values);
        }
    }

    dbo.close();
    return tab;
}

QStringList removeCommentsFromSqlList(const QStringList& sqls)
{
    QStringList result;
    for(int i=0; i<sqls.size(); i++)
    {
        if(!sqls[i].trimmed().startsWith("--"))
        {
            result.append(sqls[i]);
        }
    }

    return result;
}

bool SqliteDatabaseEngine::executeSql(Connection* c, const QStringList& sqls1, const QStringList &uid, QString& lastSql, bool rollbackOnError)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    QStringList sqls = removeCommentsFromSqlList(sqls1);
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
                db.close();
                return false;
            }
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
    return "sqlite.defaults";
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

    query.exec("select * from SQLite_Master where Type = 'trigger'");

    Trigger *result = 0;
    while(query.next())
    {
        QString trigName = query.value(1).toString();
        if(trigName == procName)
        {
            QString table =  query.value(2).toString();
            QString stmt =  query.value(4).toString();

            // brose out the stuff
            result = new Trigger(trigName, QUuid::createUuid().toString(), v);
            QString tm,ev, sql;
            parseTriggerSql(stmt, ev, tm, sql);
            result->setEvent(ev);
            result->setTable(table);
            result->setSql(sql);
            result->setTime(tm);
        }
    }
    dbo.close();
    return result;
}


/*
CREATE TRIGGER trig before  insert  on ADDRESS_1  BEGIN
select * from address_1;
END;

TODO: Who on earth wrote this code? a 2 years old? FIX FIX FIX
*/
void SqliteDatabaseEngine::parseTriggerSql(const QString &inSql, QString &outEvent, QString &outTime, QString &sql)
{
    QString currentWord = "";
    int i = 0;
    qDebug() << inSql;
    while(i<inSql.length())
    {

        // create
        while(i<inSql.length() && inSql[i].isSpace())
        {
            currentWord += inSql[i];
            i++; // skip spaces
        }


        if(i == inSql.length()) return;

        while(i < inSql.length() && ! inSql[i].isSpace()) // skip create
        {
            currentWord += inSql[i];
            i++;
        }
        currentWord = "";

        // trigger
        while(i<inSql.length() && inSql[i].isSpace()) i++; // skip spaces
        if(i == inSql.length()) return;

        while(i < inSql.length() && ! inSql[i].isSpace()) // skip trigger
        {
            currentWord += inSql[i];
            i++;
        }
        currentWord = "";

        // name
        while(i<inSql.length() && inSql[i].isSpace()) i++; // skip spaces
        if(i == inSql.length()) return;

        while(i < inSql.length() && ! inSql[i].isSpace()) // skip name
        {
            currentWord += inSql[i];
            i++;
        }
        currentWord = "";

        // time
        while(i<inSql.length() && inSql[i].isSpace()) i++; // skip spaces
        if(i == inSql.length()) return;

        while(i < inSql.length() && ! inSql[i].isSpace()) // skip and read time
        {
            currentWord += inSql[i];
            i++;
        }
        outTime = currentWord;
        currentWord = "";

        // event
        while(i<inSql.length() && inSql[i].isSpace()) i++; // skip spaces
        if(i == inSql.length()) return;

        while(i < inSql.length() && ! inSql[i].isSpace()) // skip and read event
        {
            currentWord += inSql[i];
            i++;
        }
        outEvent = currentWord;

        // "ON"
        while(i<inSql.length() && inSql[i].isSpace()) i++; // skip spaces
        if(i == inSql.length()) return;

        while(i < inSql.length() && ! inSql[i].isSpace()) // skip and read event
        {
            i++;
        }

        // table name
        while(i<inSql.length() && inSql[i].isSpace()) i++; // skip spaces
        if(i == inSql.length()) return;

        while(i < inSql.length() && ! inSql[i].isSpace()) // skip and read event
        {
            i++;
        }

        while(i<inSql.length() && inSql[i].isSpace()) i++; // skip spaces

        // fetch the BODY in, after the begin
        int begIndx = inSql.indexOf("BEGIN", i, Qt::CaseInsensitive);
        sql = inSql.mid(begIndx);
        return;
    }
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

QString SqliteDatabaseEngine::getViewCreationScript(Connection* c, const QString& name)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    QString result;

    bool ok = db.isOpen();

    if(!ok)
    {
        return result;
    }


    QSqlQuery query(db);
    query.exec(QString("select sql from sqlite_master where type='view' and name='") + name + "'");

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

QString SqliteDatabaseEngine::spiExtension(QUuid uid)
{
    if(uid.toString() == uidTemporaryTable) { return "TEMPORARY"; }
    if(uid.toString() == uidIfDoesNotExistTable) { return "if not exists"; }
    if(uid.toString() == uidColumnAutoIncrement) { return "autoincrement"; }

    return "";
}

QStringList SqliteDatabaseEngine::getKeywords() const
{
    QStringList keywordPatterns;
    keywordPatterns <<
        "ABORT" <<
        "ACTION" <<
        "ADD" <<
        "AFTER" <<
        "ALL" <<
        "ALTER" <<
        "ANALYZE" <<
        "AND" <<
        "AS" <<
        "ASC" <<
        "ATTACH" <<
        "AUTOINCREMENT" <<
        "BEFORE" <<
        "BEGIN" <<
        "BETWEEN" <<
        "BY" <<
        "CASCADE" <<
        "CASE" <<
        "CAST" <<
        "CHECK" <<
        "COLLATE" <<
        "COLUMN" <<
        "COMMIT" <<
        "CONFLICT" <<
        "CONSTRAINT" <<
        "CREATE" <<
        "CROSS" <<
        "CURRENT_DATE" <<
        "CURRENT_TIME" <<
        "CURRENT_TIMESTAMP" <<
        "DATABASE" <<
        "DEFAULT" <<
        "DEFERRABLE" <<
        "DEFERRED" <<
        "DELETE" <<
        "DESC" <<
        "DETACH" <<
        "DISTINCT" <<
        "DROP" <<
        "EACH" <<
        "ELSE" <<
        "END" <<
        "ESCAPE" <<
        "EXCEPT" <<
        "EXCLUSIVE" <<
        "EXISTS" <<
        "EXPLAIN" <<
        "FAIL" <<
        "FOR" <<
        "FOREIGN" <<
        "FROM" <<
        "FULL" <<
        "GLOB" <<
        "GROUP" <<
        "HAVING" <<
        "IF" <<
        "IGNORE" <<
        "IMMEDIATE" <<
        "IN" <<
        "INDEX" <<
        "INDEXED" <<
        "INITIALLY" <<
        "INNER" <<
        "INSERT" <<
        "INSTEAD" <<
        "INTERSECT" <<
        "INTO" <<
        "IS" <<
        "ISNULL" <<
        "JOIN" <<
        "KEY" <<
        "LEFT" <<
        "LIKE" <<
        "LIMIT" <<
        "MATCH" <<
        "NATURAL" <<
        "NO" <<
        "NOT" <<
        "NOTNULL" <<
        "NULL" <<
        "OF" <<
        "OFFSET" <<
        "ON" <<
        "OR" <<
        "ORDER" <<
        "OUTER" <<
        "PLAN" <<
        "PRAGMA" <<
        "PRIMARY" <<
        "QUERY" <<
        "RAISE" <<
        "REFERENCES" <<
        "REGEXP" <<
        "REINDEX" <<
        "RELEASE" <<
        "RENAME" <<
        "REPLACE" <<
        "RESTRICT" <<
        "RIGHT" <<
        "ROLLBACK" <<
        "ROW" <<
        "SAVEPOINT" <<
        "SELECT" <<
        "SET" <<
        "TABLE" <<
        "TEMP" <<
        "TEMPORARY" <<
        "THEN" <<
        "TO" <<
        "TRANSACTION" <<
        "TRIGGER" <<
        "UNION" <<
        "UNIQUE" <<
        "UPDATE" <<
        "USING" <<
        "VACUUM" <<
        "VALUES" <<
        "VIEW" <<
        "VIRTUAL" <<
        "WHEN" <<
        "WHERE" ;
    return keywordPatterns;
}
