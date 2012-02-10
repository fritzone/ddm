#include "dbmysql_MySQLDatabaseEngine.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>
#include <QMutexLocker>

#include "Table.h"
#include "Version.h"
#include "Column.h"
#include "Project.h"
#include "UserDataType.h"
#include "Index.h"
#include "TableInstance.h"
#include "db_AbstractStorageEngineListProvider.h"
#include "ForeignKey.h"
#include "db_DatabaseBuiltinFunction.h"
#include "core_View.h"
#include "core_Connection.h"
#include "core_Procedure.h"
#include "core_Function.h"
#include "Codepage.h"
#include "core_Trigger.h"

QVector<DatabaseBuiltinFunction>* MySQLDatabaseEngine::s_builtinFunctions = 0;
int MySQLDatabaseEngine::m_connectionCounter = 1;
QMutex* MySQLDatabaseEngine::m_connectionMutex = 0;

MySQLDatabaseEngine::MySQLDatabaseEngine() : DatabaseEngine("MySQL"), m_revEngMappings(), m_oneTimeMappings()
{
    static QVector<DatabaseBuiltinFunction> v = MySQLDatabaseEngine::buildFunctions();
    s_builtinFunctions = &v;
    m_connectionMutex = new QMutex();
    m_indexTypes << "BTREE" << "HASH" << "RTREE";
    m_defaultIndexType = "BTREE";
}

// this should be thread safe
QString MySQLDatabaseEngine::provideConnectionName(const QString& prefix)
{
    QMutexLocker lock(m_connectionMutex);
    QString t = prefix + QString::number(m_connectionCounter++);
    lock.unlock();
    return t;
}

bool MySQLDatabaseEngine::reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, const QStringList& procs,
                                                  const QStringList& funcs, const QStringList& triggers, Project*p, bool relaxed)
{
    Version* v = p->getWorkingVersion();
    m_revEngMappings.clear();
    m_oneTimeMappings.clear();

    for(int i=0; i<procs.size(); i++)
    {
        Procedure* proc = reverseEngineerProc(c, procs.at(i));
        if(proc) v->addProcedure(proc);
    }

    for(int i=0; i<funcs.size(); i++)
    {
        Function* func = reverseEngineerFunc(c, funcs.at(i));
        if(func) v->addFunction(func);
    }

    for(int i=0; i<tables.size(); i++)
    {
        Table* tab = reverseEngineerTable(c, tables.at(i), p, relaxed);
        if(tab) v->addTable(tab);
    }

    for(int i=0; i<views.size(); i++)
    {
        View* view = reverseEngineerView(c, views.at(i));
        if(view) v->addView(view);
    }

    for(int i=0; i<triggers.size(); i++)
    {
        Trigger* t= reverseEngineerTrigger(c, triggers.at(i));
        if(t) v->addTrigger(t);
    }

    // now populate the foreign keys
    {
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
                ForeignKey* fk = new ForeignKey();
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
    }
    return true;
}

QSqlDatabase MySQLDatabaseEngine::getQSqlDatabaseForConnection(Connection *c)
{
    QString newConnName = provideConnectionName("getConnection");
    QSqlDatabase dbo = QSqlDatabase::addDatabase("QMYSQL", newConnName);

    dbo.setHostName(c->getHost());
    dbo.setDatabaseName(c->getDb());
    dbo.open(c->getUser(), c->getPassword());

    return dbo;
}

QStringList MySQLDatabaseEngine::getAvailableViews(Connection* c)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

    bool ok = dbo.isOpen();
    QStringList result;

    if(!ok)
    {
        lastError = dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText();
        return result;
    }

    QSqlQuery query (dbo);

    query.exec("select table_name from information_schema.tables where table_schema='"+c->getDb()+"' and table_type='VIEW'");

    while(query.next())
    {
        QString tab = query.value(0).toString();
        result.append(tab);
    }
    dbo.close();
    return result;
}

QStringList MySQLDatabaseEngine::getAvailableTriggers(Connection* c)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

    bool ok = dbo.isOpen();
    QStringList result;

    if(!ok)
    {
        lastError = dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText();
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


QStringList MySQLDatabaseEngine::getAvailableStoredProcedures(Connection* c)
{
    QSqlDatabase dbo = c->getQSqlDatabase();

    bool ok = dbo.isOpen();

    if(!ok)
    {
        lastError = dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText();
        return QStringList();
    }

    QSqlQuery query (dbo);
    query.exec("show procedure status where Db='"+c->getDb()+"'");
    QStringList result;

    while(query.next())
    {
        QString proc = query.value(1).toString();
        result.append(proc);
    }
    dbo.close();
    return result;
}

QStringList MySQLDatabaseEngine::getAvailableStoredFunctions(Connection* c)
{
    QSqlDatabase dbo = c->getQSqlDatabase();

    bool ok = dbo.isOpen();

    if(!ok)
    {
        lastError = dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText();
        return QStringList();
    }

    QSqlQuery query (dbo);
    query.exec("show function status where Db='"+c->getDb()+"'");
    QStringList result;

    while(query.next())
    {
        QString proc = query.value(1).toString();
        result.append(proc);
    }
    dbo.close();
    return result;
}

QStringList MySQLDatabaseEngine::getAvailableTables(Connection* c)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);
    bool ok = dbo.isOpen();
    QStringList result;

    if(!ok)
    {
        lastError = dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText();
        return result;
    }

    QSqlQuery query(dbo);

    query.exec("select table_name from information_schema.tables where table_schema='"+c->getDb()+"' and table_type='BASE TABLE'");

    while(query.next())
    {
        QString tab = query.value(0).toString();
        result.append(tab);
    }
    dbo.close();
    return result;
}

Procedure* MySQLDatabaseEngine::reverseEngineerProc(Connection *c, const QString &procName)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);

    if(!db.isOpen())
    {
        return 0;
    }

    QSqlQuery query(db);
    QString t = "show create procedure " + procName;
    query.exec(t);
    Procedure* proc= 0;
    while(query.next())
    {
        QString sql = query.value(2).toString();
        proc = new Procedure(procName);
        proc->setSql(sql);
    }

    db.close();

    return proc;
}

Function* MySQLDatabaseEngine::reverseEngineerFunc(Connection *c, const QString &funcName)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);

    if(!db.isOpen())
    {
        return 0;
    }

    QSqlQuery query(db);
    QString t = "show create function " + funcName;
    query.exec(t);
    Function* func= 0;
    while(query.next())
    {
        QString sql = query.value(2).toString();
        func = new Function(funcName);
        func->setSql(sql);
    }

    db.close();

    return func;
}

View* MySQLDatabaseEngine::reverseEngineerView(Connection* c, const QString& viewName)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);


    bool ok = dbo.open();

    if(!ok)
    {
        lastError = dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText();
        return 0;
    }

    {
    QSqlQuery query(dbo);
    query.exec("SET sql_mode = 'ANSI'");
    }

    View* view = 0;
    QSqlQuery query(dbo);
    QString t = "show create view "+c->getDb()+"."+viewName;
    query.exec(t);
    while(query.next())
    {
        QString sql = query.value(1).toString();
        view = new View(true);
        view->setSql(sql);
    }
    dbo.close();
    return view;
}

QStringList MySQLDatabaseEngine::getColumnsOfTable(Connection *c, const QString &tableName)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    QStringList result;

    bool ok = db.isOpen();

    if(!ok)
    {
        return result;
    }

    QSqlQuery query(db);
    query.exec("desc " + tableName);

    int fieldNo = query.record().indexOf("Field");

    while(query.next())
    {
        QString field_name = query.value(fieldNo).toString();
        result.append(field_name);
    }
    return result;
}

Table* MySQLDatabaseEngine::reverseEngineerTable(Connection *c, const QString& tableName, Project* p, bool relaxed)
{
    QSqlDatabase dbo = c->getQSqlDatabase();
    Version* v = p->getWorkingVersion();
    bool ok = dbo.isOpen();

    if(!ok)
    {
        lastError = dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText();
        return 0;
    }

    Table* tab = new Table(v);
    tab->setName(tableName);

    // fetch all the columns of the table
    {
    QSqlQuery query(dbo);
    query.exec("desc " + tableName);

    int fieldNo = query.record().indexOf("Field");
    int typeNo = query.record().indexOf("Type");
    int nulldNo = query.record().indexOf("Null");
    int keyNo = query.record().indexOf("Key");
    int defNo = query.record().indexOf("Default");
    int extraNo = query.record().indexOf("Extra");

    while(query.next())
    {
        QString field_name = query.value(fieldNo).toString();
        QString type = query.value(typeNo).toString().toUpper();
        QString nullable = query.value(nulldNo).toString();
        QString keyness = query.value(keyNo).toString();
        QString defaultValue = query.value(defNo).toString();
        QString extra = query.value(extraNo).toString();

        UserDataType* udt = 0;
        QString oneTimeKey = field_name + type + nullable + defaultValue;
        qDebug () << "ONE TIME KEY=" << oneTimeKey;
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
        Column* col = new Column(field_name, udt, QString::compare(keyness, "PRI", Qt::CaseInsensitive) == 0, extra == "auto_increment");

        // and add the column to the table
        if(!found) m_revEngMappings.insert(udt, col);
        tab->addColumn(col);
    }
    }

    // now populate the indices of the table
    {

    QSqlQuery query(dbo);
    query.exec("show indexes from " + tableName);

    int tableNo = query.record().indexOf("Table");
    int nonuniqueNo = query.record().indexOf("Non_unique");
    int keynameNo = query.record().indexOf("Key_name");
    int seqinindexNo = query.record().indexOf("Seq_in_index");
    int columnnameNo = query.record().indexOf("Column_name");
    int collationNo = query.record().indexOf("Collation");
    int cardinalityNo = query.record().indexOf("Cardinality");
    int subpartNo = query.record().indexOf("Sub_part");
    int packedNo = query.record().indexOf("Packed");
    int nullNo = query.record().indexOf("Null");
    int indextypeNo = query.record().indexOf("Index_type");
    int commentNo = query.record().indexOf("Comment");

    QMap<QString, Index*> createdIndexes;

    while(query.next())
    {
        QString table = query.value(tableNo).toString();
        QString nonunique = query.value(nonuniqueNo).toString();
        QString keyname = query.value(keynameNo).toString();
        QString seqinindex = query.value(seqinindexNo).toString();
        QString columnname = query.value(columnnameNo).toString();
        QString collation = query.value(collationNo).toString();
        QString cardinality = query.value(cardinalityNo).toString();
        QString subpart = query.value(subpartNo).toString();
        QString packed = query.value(packedNo).toString();
        QString nulld = query.value(nullNo).toString();
        QString indextype = query.value(indextypeNo).toString();
        QString comment = query.value(commentNo).toString();

        QString finalIndexName = keyname;
        if(keyname == "PRIMARY") continue;

        Index* idx = 0;
        if(createdIndexes.keys().contains(keyname))
        {
            idx = createdIndexes[keyname];
        }
        else
        {
            idx = new Index(finalIndexName, indextype, tab);
            createdIndexes.insert(keyname, idx);
        }

        idx->addColumn(tab->getColumn(columnname), seqinindex.toInt());
        if(!tab->hasIndex(finalIndexName))
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
            inst->setValues(values);
        }
    }

    // find the storage engine
    {
        QString s = "SELECT ENGINE FROM information_schema.TABLES WHERE TABLE_SCHEMA = '"+c->getDb()+"' AND TABLE_NAME = '"+tab->getName()+"'";
        QSqlQuery query (dbo);
        query.exec(s);
        QString eng = "";
        while(query.next())
        {
            eng = query.value(0).toString();
        }

        AbstractStorageEngineListProvider* stlist = getStorageEngineListProviders();
        QVector<AbstractStorageEngine*> stengines = stlist->getStorageEngines();
        for(int i=0; i<stengines.size(); i++)
        {
            if(stengines.at(i)->name() == eng)
            {
                tab->setStorageEngine(stengines.at(i));
                break;
            }
        }
    }
    dbo.close();
    return tab;
}

bool MySQLDatabaseEngine::executeSql(Connection* c, const QStringList& sqls, QString& lastSql, bool rollbackOnError)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    bool ok = db.isOpen();
    if(!ok)
    {
        lastError = db.lastError().driverText() + "/" + db.lastError().databaseText();
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
                lastError = query.lastError().driverText() + "/" + query.lastError().databaseText();
                if(transactionSucces && rollbackOnError)
                {
                    db.rollback();
                }
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

QString MySQLDatabaseEngine::getDefaultDatatypesLocation()
{
    return "mysql.defaults";
}

QStringList MySQLDatabaseEngine::getAvailableDatabases(const QString& host, const QString& user, const QString& pass)
{
    Connection *c = new Connection("temp", host, user, pass, "", false, false);
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    bool ok = db.isOpen();
    QStringList result;

    if(!ok)
    {
        lastError = QObject::tr("Cannot connect to the database: ") + db.lastError().databaseText() + "/" + db.lastError().driverText();
        return result;
    }

    QSqlQuery query(db);
    query.exec("show databases");

    while(query.next())
    {
        QString dbName = query.value(0).toString();
        result.append(dbName);
    }
    db.close();

    return result;
}

bool MySQLDatabaseEngine::dropDatabase(Connection* c)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    bool ok = db.isOpen();
    if(!ok)
    {
        lastError = QObject::tr("Cannot connect to the database: ") + db.lastError().databaseText() + "/" + db.lastError().driverText();
        return false;
    }

    QSqlQuery query(db);
    bool t = query.exec("drop database "+ c->getDb());
    if(!t)
    {
        lastError = QObject::tr("Cannot drop a database: ") + db.lastError().databaseText() + "/" + db.lastError().driverText();
        return false;
    }
    db.close();
    c->setState(DROPPED);
    return true;
}

bool MySQLDatabaseEngine::createDatabase(Connection* c)
{
    QString newConnName = provideConnectionName("getConnection");
    QSqlDatabase dbo = QSqlDatabase::addDatabase("QMYSQL", newConnName);

    dbo.setHostName(c->getHost());
    dbo.open(c->getUser(), c->getPassword());

    bool ok = dbo.isOpen();
    if(!ok)
    {
        lastError = QObject::tr("Cannot connect to the database: ") + dbo.lastError().databaseText() + "/" + dbo.lastError().driverText();
        return false;
    }

    QSqlQuery query(dbo);
    bool t = query.exec("create database "+ c->getDb());
    if(!t)
    {
        lastError = QObject::tr("Cannot create a database: ") + dbo.lastError().databaseText() + "/" + dbo.lastError().driverText();
        return false;
    }
    dbo.close();
    return true;
}

QVector<DatabaseBuiltinFunction> MySQLDatabaseEngine::getBuiltinFunctions()
{
    return *s_builtinFunctions;
}


QVector<DatabaseBuiltinFunction> MySQLDatabaseEngine::buildFunctions()
{
    static QVector<DatabaseBuiltinFunction> result;
    QString X = QString("X");


#define RET_NUMERIC     UserDataType("return", DT_NUMERIC)
#define RET_DATETIME    UserDataType("return", DT_DATETIME)
#define RET_STRING      UserDataType("return", DT_STRING)

#define PAR_STRING      DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_STRING), true)
#define PAR_NUMERIC     DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_NUMERIC), true)
#define PAR_VARIABLE    DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_VARIABLE), true)

#define OPAR_STRING      DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_STRING), false)
#define OPAR_NUMERIC     DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DT_NUMERIC), false)

#define FUNC result.append(DatabaseBuiltinFunction(QString

    FUNC("@if"),           FT_CONTROLFLOW,  RET_STRING, PAR_STRING, PAR_STRING, PAR_STRING, "IF(expr1,expr2,expr3) If expr1 is TRUE (expr1 <> 0 and expr1 <> NULL) then IF() returns expr2; otherwise it returns expr3. IF() returns a numeric or string value, depending on the context in which it is used."));
    FUNC("@ifnull"),       FT_CONTROLFLOW,  RET_STRING, PAR_STRING, PAR_STRING, "IFNULL(expr1,expr2) If expr1 is not NULL, IFNULL() returns expr1; otherwise it returns expr2. IFNULL() returns a numeric or string value, depending on the context in which it is used."));
    FUNC("@nullif"),       FT_CONTROLFLOW,  RET_STRING, PAR_STRING, PAR_STRING, "NULLIF(expr1,expr2) Returns NULL if expr1 = expr2 is true, otherwise returns expr1."));

    FUNC("@abs"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ABS(X) Returns the absolute value of X"));
    FUNC("@acos"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ACOS(X) Returns the arc cosine of X, that is, the value whose cosine is X. Returns NULL if X is not in the range -1 to 1. "));
    FUNC("@asin"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ASIN(X) Returns the arc sine of X, that is, the value whose sine is X. Returns NULL if X is not in the range -1 to 1. "));
    FUNC("@atan"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ATAN(X) Returns the arc tangent of X, that is, the value whose tangent is X."));
    FUNC("@ceiling"),      FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "CEILING(X) Returns the smallest integer value not less than X."));
    FUNC("@conv"),         FT_NUMERIC,   RET_STRING,  PAR_STRING, PAR_NUMERIC, PAR_NUMERIC, "CONV(X,from_base,to_base) Converts numbers between different number bases. Returns a string representation of the number N, converted from base from_base to base to_base."));
    FUNC("@cos"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "COS(X) Returns the cosine of X, where X is given in radians. "));
    FUNC("@cot"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "COT(X) Returns the cotangent of X."));
    FUNC("@crc32"),        FT_NUMERIC,   RET_NUMERIC, PAR_STRING,  "CRC32(str) Computes a cyclic redundancy check value and returns a 32-bit unsigned value."));
    FUNC("@degrees"),      FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "DEGREES(X) Returns the argument X, converted from radians to degrees. "));
    FUNC("@exp"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "EXP(X) Returns the value of e (the base of natural logarithms) raised to the power of X."));
    FUNC("@floor"),        FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "FLOOR(X) Returns the largest integer value not greater than X."));
    FUNC("@ln"),           FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "LN(X) Returns the natural logarithm of X; that is, the base-e logarithm of X."));
    FUNC("@log"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, PAR_NUMERIC, "LOG(B,X) Returns the logarithm of X to the base B"));
    FUNC("@log2"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "LOG2(X) Returns the 2-base logarithm of X"));
    FUNC("@log10"),        FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "LOG10(X) Returns the 10-base logarithm of X"));
    FUNC("@pi"),           FT_NUMERIC,   RET_NUMERIC, "PI() Returns the value of p (pi)."));
    FUNC("@pow"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, PAR_NUMERIC, "POW(X,Y) Returns the value of X raised to the power of Y."));
    FUNC("@radians"),      FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "RADIANS(X) Returns the argument X, converted from degrees to radians. (Note that π radians equals 180 degrees.)"));
    FUNC("@rand"),         FT_NUMERIC,   RET_NUMERIC, OPAR_NUMERIC, "RAND([N]) Returns a random floating-point value v in the range 0 <= v < 1.0. If a constant integer argument N is specified, it is used as the seed value."));
    FUNC("@round"),        FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, OPAR_NUMERIC, "ROUND(X,D) Rounds the argument X to D decimal places."));
    FUNC("@sign"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "SIGN(X) Returns the sign of the argument as -1, 0, or 1, depending on whether X is negative, zero, or positive."));
    FUNC("@sin"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "SIN(X) Returns the sine of X, where X is given in radians."));
    FUNC("@sqrt"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "SQRT(X) Returns the square root of a nonnegative number X."));
    FUNC("@tan"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "TAN(X) Returns the tangent of X, where X is given in radians."));
    FUNC("@truncate"),     FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, OPAR_NUMERIC, "TRUNCATE(X,D) Returns the number X, truncated to D decimal places."));

    FUNC("@adddate"),      FT_DATETIME,  RET_STRING, PAR_STRING, PAR_NUMERIC, "ADDDATE(expr,days) Adds the given days to the date."));
    FUNC("@addtime"),      FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "ADDTIME(expr1,expr2) Adds expr2 to expr1 and returns the result. expr1 is a time or datetime expression, and expr2 is a time expression."));
    FUNC("@addtime"),      FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, PAR_STRING, "CONVERT_TZ(dt,from_tz,to_tz) CONVERT_TZ() converts a datetime value dt from the time zone given by from_tz to the time zone given by to_tz and returns the resulting value."));
    FUNC("@curdate"),      FT_DATETIME,  RET_STRING, "CURDATE() Returns the current date as a value in 'YYYY-MM-DD' or YYYYMMDD format, depending on whether the function is used in a string or numeric context."));
    FUNC("@curtime"),      FT_DATETIME,  RET_STRING, "CURTIME() Returns the current time as a value in 'HH:MM:SS' or HHMMSS.uuuuuu format, depending on whether the function is used in a string or numeric context. The value is expressed in the current time zone."));
    FUNC("@date"),         FT_DATETIME,  RET_STRING, PAR_STRING, "DATE(expr) Extracts the date part of the date or datetime expression expr."));
    FUNC("@datediff"),     FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "DATEDIFF(expr1,expr2) Returns expr1 – expr2 expressed as a value in days from one date to the other. expr1 and expr2 are date or date-and-time expressions. Only the date parts of the values are used in the calculation. "));
    FUNC("@date_add"),     FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "DATE_ADD(date,INTERVAL expr unit) Performs date arithmetic. The date argument specifies the starting date or datetime value. expr is an expression specifying the interval value to be added or subtracted from the starting date."));
    FUNC("@date_sub"),     FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "DATE_SUB(date,INTERVAL expr unit) Performs date arithmetic. The date argument specifies the starting date or datetime value. expr is an expression specifying the interval value to be added or subtracted from the starting date."));
    FUNC("@date_format"),  FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "DATE_FORMAT(date,expr) Formats the date value according to the format string."));
    FUNC("@dayname"),      FT_DATETIME,  RET_STRING, PAR_STRING, "DAYNAME(date) Returns the name of the weekday for date."));
    FUNC("@dayofmonth"),   FT_DATETIME,  RET_STRING, PAR_STRING, "DAYOFMONTH(date) Returns the day of the month for date, in the range 1 to 31, or 0 for dates such as '0000-00-00' or '2008-00-00' that have a zero day part."));
    FUNC("@dayofweek"),    FT_DATETIME,  RET_STRING, PAR_STRING, "DAYOFWEEK(date) Returns the weekday index for date (1 = Sunday, 2 = Monday, ... , 7 = Saturday)."));
    FUNC("@dayofyear"),    FT_DATETIME,  RET_STRING, PAR_STRING, "DAYOFYEAR(date) Returns the day of the year for date, in the range 1 to 366."));
    FUNC("@extract"),      FT_DATETIME,  RET_STRING, PAR_STRING, "EXTRACT(unit FROM date) The EXTRACT() function uses the same kinds of unit specifiers as DATE_ADD() or DATE_SUB(), but extracts parts from the date rather than performing date arithmetic."));
    FUNC("@from_days"),    FT_DATETIME,  RET_STRING, PAR_STRING, "FROM_DAYS(N) Given a day number N, returns a DATE value. "));
    FUNC("@from_unixtime"),FT_DATETIME,  RET_STRING, PAR_NUMERIC, OPAR_STRING, "FROM_UNIXTIME(N,[FORMAT]) Returns a representation of the unix_timestamp argument as a value in 'YYYY-MM-DD HH:MM:SS' or YYYYMMDDHHMMSS.uuuuuu format, depending on whether the function is used in a string or numeric context. If format is given, the result is formatted according to the format string."));
    FUNC("@get_format"),   FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "GET_FORMAT({DATE|TIME|DATETIME}, {'EUR'|'USA'|'JIS'|'ISO'|'INTERNAL'}) Returns a format string. This function is useful in combination with the DATE_FORMAT() and the STR_TO_DATE() functions."));
    FUNC("@hour"),         FT_DATETIME,  RET_NUMERIC, PAR_STRING, "HOUR(time) Returns the hour for time."));
    FUNC("@last_day"),     FT_DATETIME,  RET_STRING, PAR_STRING, "LAST_DAY(date) Takes a date or datetime value and returns the corresponding value for the last day of the month."));
    FUNC("@makedate"),     FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "MAKEDATE(year,dayofyear) Returns a date, given year and day-of-year values. dayofyear must be greater than 0 or the result is NULL."));
    FUNC("@maketime"),     FT_DATETIME,  RET_STRING, PAR_NUMERIC, PAR_NUMERIC, PAR_NUMERIC, "MAKETIME(hour,min,second) Returns a time value calculated from the hour, minute, and second arguments."));
    FUNC("@microsecond"),  FT_DATETIME,  RET_NUMERIC, PAR_STRING, "MICROSECOND(expr) Returns the microseconds from the time or datetime expression expr as a number in the range from 0 to 999999."));
    FUNC("@minute"),       FT_DATETIME,  RET_NUMERIC, PAR_STRING, "MINUTE(time) Returns the minute for time."));
    FUNC("@month"),        FT_DATETIME,  RET_NUMERIC, PAR_STRING, "MONTH(date) Returns the month for date, in the range 1 to 12 for January to December, or 0 for dates such as '0000-00-00' or '2008-00-00' that have a zero month part."));
    FUNC("@monthname"),    FT_DATETIME,  RET_STRING, PAR_STRING, "MONTHNAME(date) Returns the full name of the month for date."));
    FUNC("@now"),          FT_DATETIME,  RET_STRING, "Returns the current date and time as a value in 'YYYY-MM-DD HH:MM:SS' or YYYYMMDDHHMMSS.uuuuuu format"));
    FUNC("@period_add"),   FT_DATETIME,  RET_STRING, PAR_STRING, PAR_NUMERIC, "PERIOD_ADD(P,N) Adds N months to period P (in the format YYMM or YYYYMM). Returns a value in the format YYYYMM. Note that the period argument P is not a date value."));
    FUNC("@period_diff"),  FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "PERIOD_DIFF(P1,P2) Returns the number of months between periods P1 and P2. P1 and P2 should be in the format YYMM or YYYYMM. Note that the period arguments P1 and P2 are not date values."));
    FUNC("@quarter"),      FT_DATETIME,  RET_STRING, PAR_STRING, "QUARTER(date) Returns the quarter of the year for date, in the range 1 to 4."));
    FUNC("@second"),       FT_DATETIME,  RET_NUMERIC, PAR_STRING, "SECOND(time) Returns the second of the time in range 0 to 59."));
    FUNC("@sec_to_time"),  FT_DATETIME,  RET_STRING, PAR_STRING, "SEC_TO_TIME(seconds) Returns the seconds argument, converted to hours, minutes, and seconds, as a TIME value."));
    FUNC("@str_to_date"),  FT_DATETIME,  RET_DATETIME, PAR_STRING, PAR_STRING, "STR_TO_DATE(str,format) Returns a DATETIME value if the format string contains both date and time parts, or a DATE or TIME value if the string contains only date or time parts."));
    FUNC("@subdate"),      FT_DATETIME,  RET_DATETIME, PAR_STRING, PAR_NUMERIC, "SUBDATE(expr,days) Returns the date value obtained by substracting days from expr."));
    FUNC("@subtime"),      FT_DATETIME,  RET_DATETIME, PAR_STRING, PAR_STRING, "SUBTIME(expr1,expr2) Returns expr1 – expr2 expressed as a value in the same format as expr1. expr1 is a time or datetime expression, and expr2 is a time expression."));
    FUNC("@sysdate"),      FT_DATETIME,  RET_STRING, "Returns the current date and time as a value in 'YYYY-MM-DD HH:MM:SS' or YYYYMMDDHHMMSS.uuuuuu format, depending on whether the function is used in a string or numeric context."));
    FUNC("@time"),         FT_DATETIME,  RET_NUMERIC, PAR_STRING, "TIME(expr) Extracts the time part of the time or datetime expression expr and returns it as a string."));
    FUNC("@timediff"),     FT_DATETIME,  RET_DATETIME, PAR_STRING, PAR_STRING, "TIMEDIFF(expr1,expr2) Returns expr1 – expr2 expressed as a time value. expr1 and expr2 are time or date-and-time expressions, but both must be of the same type."));
    FUNC("@timestamp"),    FT_DATETIME,  RET_DATETIME, PAR_STRING, PAR_STRING, "TIMESTAMP(expr1,expr2) It adds the time expression expr2 to the date or datetime expression expr1 and returns the result as a datetime value."));
    FUNC("@timestamp"),    FT_DATETIME,  RET_DATETIME, PAR_STRING, "TIMESTAMP(expr) Returns the date or datetime expression expr as a datetime value."));
    FUNC("@timestampadd"), FT_DATETIME,  RET_DATETIME, PAR_STRING, PAR_NUMERIC, PAR_STRING, "TIMESTAMPADD(unit,interval,datetime_expr) Adds the integer expression interval to the date or datetime expression datetime_expr. The unit for interval is given by the unit argument, which should be one of the following values: FRAC_SECOND (microseconds), SECOND, MINUTE, HOUR, DAY, WEEK, MONTH, QUARTER, or YEAR."));
    FUNC("@timestampdiff"),FT_DATETIME,  RET_DATETIME, PAR_STRING, PAR_STRING, PAR_STRING, "TIMESTAMPDIFF(unit,datetime_expr1,datetime_expr2) Returns datetime_expr2 – datetime_expr1, where datetime_expr1 and datetime_expr2 are date or datetime expressions."));
    FUNC("@time_format"),  FT_DATETIME,  RET_STRING, PAR_STRING, PAR_STRING, "TIME_FORMAT(time,expr) Formats the time value according to the format string."));
    FUNC("@time_to_sec"),  FT_DATETIME,  RET_NUMERIC, PAR_STRING, "TIME_TO_SEC(time) Returns the time argument, converted to seconds."));
    FUNC("@to_days"),      FT_DATETIME,  RET_NUMERIC, PAR_STRING, "TO_DAYS(date) Given a date date, returns a day number (the number of days since year 0)."));
    FUNC("@unix_timestamp"),FT_DATETIME, RET_NUMERIC, "UNIX_TIMESTAMP() Returns a Unix timestamp (seconds since '1970-01-01 00:00:00' UTC) as an unsigned integer"));
    FUNC("@unix_timestamp"),FT_DATETIME, RET_NUMERIC, PAR_STRING, "UNIX_TIMESTAMP(date) Returns returns the value of the argument seconds since '1970-01-01 00:00:00' UTC as an unsigned integer"));
    FUNC("@utc_date"),     FT_DATETIME,  RET_STRING, "UTC_DATE() Returns the current UTC date as a value in 'YYYY-MM-DD' or YYYYMMDD format, depending on whether the function is used in a string or numeric context."));
    FUNC("@utc_time"),     FT_DATETIME,  RET_STRING, "UTC_TIME() Returns the current UTC time as a value in 'HH:MM:SS' or HHMMSS.uuuuuu format, depending on whether the function is used in a string or numeric context."));
    FUNC("@utc_timestamp"),FT_DATETIME,  RET_STRING, "UTC_TIMESTAMP() Returns the current UTC date and time as a value in 'YYYY-MM-DD HH:MM:SS' or YYYYMMDDHHMMSS.uuuuuu format, depending on whether the function is used in a string or numeric context."));
    FUNC("@week"),         FT_DATETIME,  RET_NUMERIC, PAR_STRING,  "WEEK(date) This function returns the week number for date."));
    FUNC("@weekday"),      FT_DATETIME,  RET_NUMERIC, PAR_STRING,  "WEEKDAY(date) This function returns the weekday index for date."));
    FUNC("@weekofyear"),   FT_DATETIME,  RET_NUMERIC, PAR_STRING,  "WEEKOFYEAR(date) Returns the calendar week of the date as a number in the range from 1 to 53."));
    FUNC("@year"),         FT_DATETIME,  RET_NUMERIC, PAR_STRING,  "YEAR(date) Returns the year for date, in the range 1000 to 9999, or 0 for the “zero” date."));
    FUNC("@yearweek"),     FT_DATETIME,  RET_NUMERIC, PAR_STRING,  "YEARWEEK(date) Returns year and week for a date."));

    FUNC("@avg"),          FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "AVG(expr) Returns the average value of expr."));
    FUNC("@bit_and"),      FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "BIT_AND(X) Returns the bitwise AND of all bits in expr. The calculation is performed with 64-bit (BIGINT) precision. "));
    FUNC("@bit_or"),       FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "BIT_XOR(X) Returns the bitwise OR of all bits in expr. The calculation is performed with 64-bit (BIGINT) precision. "));
    FUNC("@bit_xor"),      FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "BIT_XOR(X) Returns the bitwise XOR of all bits in expr. The calculation is performed with 64-bit (BIGINT) precision. "));
    FUNC("@count"),        FT_AGGREGATE, RET_NUMERIC, PAR_VARIABLE, "COUNT([DISTINCT]expr) Returns a count of the number of non-NULL values of expr in the rows retrieved by a SELECT statement. The result is a BIGINT value."));
    FUNC("@group_concat"), FT_AGGREGATE, RET_STRING,  PAR_VARIABLE, "GROUP_CONCAT(expr) Returns a string result with the concatenated non-NULL values from a group. It returns NULL if there are no non-NULL values. "));
    FUNC("@max"),          FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "MAX(expr) Returns the max value of expr."));
    FUNC("@min"),          FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "MIN(expr) Returns the min value of expr."));
    FUNC("@std"),          FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "STD(expr) Returns the population standard deviation of expr."));
    FUNC("@stddev"),       FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "STDDEV(expr) Returns the population standard deviation of expr."));
    FUNC("@stddev_pop"),   FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "STDDEV_POP(expr) Returns the population standard deviation of expr."));
    FUNC("@stddev_samp"),  FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "STDDEV_SAMP(expr) Returns the sample standard deviation of expr."));
    FUNC("@umd"),          FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "SUM(expr) Returns the sum of expr."));
    FUNC("@var_pop"),      FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "VAR_POP(expr) Returns the population standard variance of expr."));
    FUNC("@var_samp"),     FT_AGGREGATE, RET_NUMERIC, PAR_NUMERIC,  "VAR_SAMP(expr) Returns the sample variance of expr."));


    FUNC("@ascii"),        FT_STRING,   RET_NUMERIC,  PAR_STRING, "ASCII(str) Returns the numeric value of the leftmost character of the string str. Returns 0 if str is the empty string. Returns NULL if str is NULL. ASCII() works for 8-bit characters."));
    FUNC("@bin"),          FT_STRING,   RET_STRING,   PAR_NUMERIC, "BIN(N) Returns a string representation of the binary value of N, where N is a longlong (BIGINT) number. This is equivalent to CONV(N,10,2). Returns NULL if N is NULL."));
    FUNC("@bit_length"),   FT_STRING,   RET_NUMERIC,  PAR_STRING, "BIT_LENGTH(str) Returns the length of the string str in bits."));
    FUNC("@char"),         FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_VARIABLE,  "CHAR(N,...) interprets each argument N as an integer and returns a string consisting of the characters given by the code values of those integers. NULL values are skipped."));
    FUNC("@char_length"),  FT_STRING,   RET_NUMERIC,  PAR_STRING, "CHAR_LENGTH(str) Returns the length of the string str, measured in characters. A multi-byte character counts as a single character."));
    FUNC("@concat"),       FT_STRING,   RET_STRING,   PAR_VARIABLE, "CONCAT(str1,str2,...) Returns the string that results from concatenating the arguments. May have one or more arguments."));
    FUNC("@concat_ws"),    FT_STRING,   RET_STRING,   PAR_VARIABLE, "CONCAT_WS(separator,str1,str2,...) stands for Concatenate With Separator and is a special form of CONCAT(). The first argument is the separator for the rest of the arguments. The separator is added between the strings to be concatenated."));
    FUNC("@elt"),          FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_VARIABLE, "ELT(N,str1,str2,str3,...) Returns str1 if N = 1, str2 if N = 2, and so on. Returns NULL if N is less than 1 or greater than the number of arguments."));
    FUNC("@export_set"),   FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_STRING, PAR_STRING, OPAR_STRING, OPAR_NUMERIC, "EXPORT_SET(bits,on,off[,separator[,number_of_bits]]) Returns a string such that for every bit set in the value bits, you get an on string and for every bit not set in the value, you get an off string. "));
    FUNC("@field"),        FT_STRING,   RET_NUMERIC,  PAR_VARIABLE, "FIELD(str,str1,str2,str3,...) Returns the index (position) of str in the str1, str2, str3, ... list. Returns 0 if str is not found. "));
    FUNC("@find_in_set"),  FT_STRING,   RET_NUMERIC,  PAR_STRING, PAR_STRING, "FIND_IN_SET(str,strlist) Returns a value in the range of 1 to N if the string str is in the string list strlist consisting of N substrings."));
    FUNC("@format"),       FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_NUMERIC, "FORMAT(X,D) Formats the number X to a format like '#,###,###.##', rounded to D decimal places, and returns the result as a string. If D is 0, the result has no decimal point or fractional part. D should be a constant value. "));
    FUNC("@hex"),          FT_STRING,   RET_STRING,   PAR_NUMERIC, "HEX(N) For a numeric argument N, HEX() returns a hexadecimal string representation of the value of N treated as a longlong (BIGINT) number."));
    FUNC("@hex"),          FT_STRING,   RET_STRING,   PAR_STRING, "HEX(str) For a string argument str, HEX() returns a hexadecimal string representation of str where each character in str is converted to two hexadecimal digits."));
    FUNC("@insert"),       FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, PAR_NUMERIC, PAR_STRING, "INSERT(str,pos,len,newstr) Returns the string str, with the substring beginning at position pos and len characters long replaced by the string newstr."));
    FUNC("@instr"),        FT_STRING,   RET_NUMERIC,  PAR_STRING, PAR_STRING, "INSTR(str,substr) Returns the position of the first occurrence of substring substr in string str."));
    FUNC("@left"),         FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, "LEFT(str,len) Returns the leftmost len characters from the string str, or NULL if any argument is NULL."));
    FUNC("@length"),       FT_STRING,   RET_NUMERIC,  PAR_STRING, "LENGTH(str) Returns the length of the string str, measured in bytes."));
    FUNC("@load_file"),    FT_STRING,   RET_STRING,   PAR_STRING, "LOAD_FILE(file_name) Reads the file and returns the file contents as a string."));
    FUNC("@locate"),       FT_STRING,   RET_STRING,   PAR_STRING, PAR_STRING, "LOCATE(substr,str) Returns the position of the first occurrence of substring substr in string str."));
    FUNC("@locate"),       FT_STRING,   RET_STRING,   PAR_STRING, PAR_STRING, PAR_NUMERIC, "LOCATE(substr,str,pos) Returns the position of the first occurrence of substring substr in string str starting at position pos."));
    FUNC("@lower"),        FT_STRING,   RET_STRING,   PAR_STRING, "LOWER(str) Returns the string str with all characters changed to lowercase according to the current character set mapping."));
    FUNC("@lpad"),         FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, PAR_STRING, "LPAD(str,len,padstr) Returns the string str, left-padded with the string padstr to a length of len characters. If str is longer than len, the return value is shortened to len characters. "));
    FUNC("@ltrim"),        FT_STRING,   RET_STRING,   PAR_STRING, "LTRIM(str) Returns the string str with leading space characters removed. "));
    FUNC("@make_set"),     FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_VARIABLE, "MAKE_SET(bits,str1,str2,...) Returns a set value (a string containing substrings separated by “,” characters) consisting of the strings that have the corresponding bit in bits set. str1 corresponds to bit 0, str2 to bit 1, and so on."));
    FUNC("@mid"),          FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, PAR_NUMERIC, "MID(str,pos,len) is a synonym for SUBSTRING(str,pos,len). "));
    FUNC("@oct"),          FT_STRING,   RET_STRING,   PAR_NUMERIC, "OCT(N) Returns a string representation of the octal value of N, where N is a longlong (BIGINT) number."));
    FUNC("@ord"),          FT_STRING,   RET_NUMERIC,  PAR_STRING, "ORD(str) returns the same value as the ASCII() function."));
    FUNC("@quote"),        FT_STRING,   RET_STRING,   PAR_STRING, "QUOTE(str) Quotes a string to produce a result that can be used as a properly escaped data value in an SQL statement. "));
    FUNC("@repeat"),       FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, "REPEAT(str,count) Returns a string consisting of the string str repeated count times. If count is less than 1, returns an empty string. Returns NULL if str or count are NULL."));
    FUNC("@replace"),      FT_STRING,   RET_STRING,   PAR_STRING, PAR_STRING, PAR_STRING, "REPLACE(str,from_str,to_str) Returns the string str with all occurrences of the string from_str replaced by the string to_str."));
    FUNC("@reverse"),      FT_STRING,   RET_STRING,   PAR_STRING, "REVERSE(str) Returns the string str with the order of the characters reversed."));
    FUNC("@right"),        FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, "RIGHT(str,len) Returns the rightmost len characters from the string str, or NULL if any argument is NULL."));
    FUNC("@rpad"),         FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, "RPAD(str,len,padstr) Returns the string str, right-padded with the string padstr to a length of len characters. If str is longer than len, the return value is shortened to len characters."));
    FUNC("@rtrim"),        FT_STRING,   RET_STRING,   PAR_STRING, "RTRIM(str) Returns the string str with trailing space characters removed. "));
    FUNC("@soundex"),      FT_STRING,   RET_STRING,   PAR_STRING, "SOUNDEX(str) Returns a soundex string from str. Two strings that sound almost the same should have identical soundex strings. A standard soundex string is four characters long, but the SOUNDEX() function returns an arbitrarily long string."));
    FUNC("@space"),        FT_STRING,   RET_STRING,   PAR_NUMERIC, "SPACE(N) Returns a string consisting of N space characters."));
    FUNC("@substring"),    FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, OPAR_NUMERIC, "SUBSTRING(str,pos,len) Return a substring len characters long from string str, starting at position pos."));
    FUNC("@substring_index"),FT_STRING,RET_STRING,   PAR_STRING, PAR_STRING, PAR_NUMERIC, "SUBSTRING_INDEX(str,delim,count) Returns the substring from string str before count occurrences of the delimiter delim."));
    FUNC("@trim"),         FT_STRING,   RET_STRING,   OPAR_STRING, PAR_STRING, "TRIM([remstr FROM] str) Returns the string str with all remstr prefixes or suffixes removed."));
    FUNC("@unhex"),        FT_STRING,   RET_STRING,   PAR_STRING, "UNHEX(str) For a string argument str, UNHEX(str) performs the inverse operation of HEX(str)."));
    FUNC("@upper"),        FT_STRING,   RET_STRING,   PAR_STRING, "UPPER(str) Returns the string str with all characters changed to uppercase according to the current character set mapping."));

    FUNC("@convert"),      FT_CAST,     RET_STRING,   PAR_STRING, PAR_STRING, "CONVERT(expr,type) The CONVERT() function takes a value of one type and produces a value of another type. See the online help for more info."));

    FUNC("@bit_count"),    FT_BIT,      RET_NUMERIC,  PAR_NUMERIC, "BIT_COUNT(N) Returns the number of bits that are set in the argument N. "));

    FUNC("@aes_decrypt"),  FT_CRYPT,    RET_STRING,   PAR_STRING, PAR_STRING, "AES_DECRYPT(crypt_str,key_str) This function decrypts data using the official AES (Advanced Encryption Standard) algorithm."));
    FUNC("@aes_encrypt"),  FT_CRYPT,    RET_STRING,   PAR_STRING, PAR_STRING, "AES_ENCRYPT(str,key_str) This function encrypts a string and returns a binary string using the official AES (Advanced Encryption Standard) algorithm."));
    FUNC("@compress"),     FT_CRYPT,    RET_STRING,   PAR_STRING, "COMPRESS(str) Compresses a string and returns the result as a binary string."));
    FUNC("@decode"),       FT_CRYPT,    RET_STRING,   PAR_STRING, PAR_STRING, "DECODE(crypt_str,pass_str) Decrypts the encrypted string crypt_str using pass_str as the password. crypt_str should be a string returned from ENCODE()."));
    FUNC("@des_decrypt"),  FT_CRYPT,    RET_STRING,   PAR_STRING, OPAR_STRING, "DES_DECRYPT(crypt_str,pass_str) Decrypts a string encrypted with DES_ENCRYPT()."));
    FUNC("@des_encrypt"),  FT_CRYPT,    RET_STRING,   PAR_STRING, OPAR_STRING, "DES_ENCRYPT(str,[key]) Encrypts the string with the given key using the Triple-DES algorithm."));
    FUNC("@encode"),       FT_CRYPT,    RET_STRING,   PAR_STRING, PAR_STRING, "ENCODE(str,pass_str) Encrypt str using pass_str as the password. To decrypt the result, use DECODE()."));
    FUNC("@encrypt"),      FT_CRYPT,    RET_STRING,   PAR_STRING, OPAR_STRING, "ENCRYPT(str,[salt]) Encrypts str using the Unix crypt() system call and returns a binary string."));
    FUNC("@md5"),          FT_CRYPT,    RET_STRING,   PAR_STRING, "MD5(str) Calculates an MD5 128-bit checksum for the string. The value is returned as a binary string of 32 hex digits, or NULL if the argument was NULL."));
    FUNC("@old_password"), FT_CRYPT,    RET_STRING,   PAR_STRING, "OLD_PASSWORD(str) Returns the value of the pre-4.1 implementation of PASSWORD() as a binary string, and is intended to permit you to reset passwords for any pre-4.1 clients that need to connect to your version 5.0 MySQL server without locking them out."));
    FUNC("@password"),     FT_CRYPT,    RET_STRING,   PAR_STRING, "PASSWORD(str) Calculates and returns a password string from the plaintext password str and returns a binary string, or NULL if the argument was NULL."));
    FUNC("@sha"),          FT_CRYPT,    RET_STRING,   PAR_STRING, "SHA(str) Calculates an SHA-1 160-bit checksum for the string, as described in RFC 3174 (Secure Hash Algorithm). The value is returned as a binary string of 40 hex digits, or NULL if the argument was NULL."));
    FUNC("@uncompress"),   FT_CRYPT,    RET_STRING,   PAR_STRING, "UNCOMPRESS(str) Uncompresses a string compressed by the COMPRESS() function."));
    FUNC("@uncompressed_length"),FT_CRYPT,    RET_NUMERIC,   PAR_STRING, "UNCOMPRESSED:LENGTH(str) Returns the length that the compressed string had before being compressed."));

    FUNC("@benchmark"),    FT_INFO,    RET_NUMERIC,   PAR_NUMERIC, PAR_STRING, "BENCHMARK(count,expr) The BENCHMARK() function executes the expression expr repeatedly count times. It may be used to time how quickly MySQL processes the expression. The result value is always 0."));
    FUNC("@charset"),      FT_INFO,    RET_STRING,    PAR_STRING, "CHARSET(str) Returns the character set of the string argument."));
    FUNC("@coercibilty"),  FT_INFO,    RET_STRING,    PAR_STRING, "COERCIBILITY(str) Returns the collation coercibility value of the string argument."));
    FUNC("@collation"),    FT_INFO,    RET_STRING,    PAR_STRING, "COLLATION(str) Returns the collation of the string argument."));
    FUNC("@connection_id"),FT_INFO,    RET_NUMERIC,               "CONNECTION_ID() Returns the connection ID (thread ID) for the connection. Every connection has an ID that is unique among the set of currently connected clients. "));
    FUNC("@current_user"), FT_INFO,    RET_STRING,                "CURRENT_USER() Returns the user name and host name combination for the MySQL account that the server used to authenticate the current client."));
    FUNC("@database"),     FT_INFO,    RET_STRING,                "DATABASE() Returns the default (current) database name as a string in the utf8 character set. If there is no default database, DATABASE() returns NULL."));
    FUNC("@found_rows"),   FT_INFO,    RET_NUMERIC,               "FOUND_ROWS() To obtain how many rows the statement would have returned without the LIMIT, include a SQL_CALC_FOUND_ROWS option in the SELECT statement, and then invoke FOUND_ROWS()."));
    FUNC("@last_insert_id"),FT_INFO,   RET_NUMERIC,               "LAST_INSERT_ID() Returns the first automatically generated value that was set for an AUTO_INCREMENT column by the most recently executed INSERT statement to affect such a column."));
    FUNC("@row_count"),    FT_INFO,    RET_NUMERIC,               "ROW_COUNT() Returns the number of rows changed, deleted, or inserted by the last statement if it was an UPDATE, DELETE, or INSERT."));
    FUNC("@schema"),       FT_INFO,    RET_STRING,                "SCHEMA() is a synonym for DATABASE()."));
    FUNC("@session_user"), FT_INFO,    RET_STRING,                "SESSION_USER() is a synonym for USER()."));
    FUNC("@system_user"),  FT_INFO,    RET_STRING,                "SYSTEM_USER() is a synonym for USER()."));
    FUNC("@user"),         FT_INFO,    RET_STRING,                "USER() Returns the current MySQL user name and host name as a string in the utf8 character set."));
    FUNC("@version"),      FT_INFO,    RET_STRING,                "VERSION() Returns a string that indicates the MySQL server version."));

    FUNC("@default"),      FT_MISC,    RET_STRING,    PAR_STRING, "DEFAULT(col_name) Returns the default value for a table column."));
    FUNC("@format"),       FT_MISC,    RET_STRING,    PAR_NUMERIC, PAR_STRING, "FORMAT(X,D) Formats the number X to a format like '#,###,###.##', rounded to D decimal places, and returns the result as a string."));
    FUNC("@get_lock"),     FT_MISC,    RET_NUMERIC,   PAR_STRING,  PAR_NUMERIC, "GET_LOCK(str,timeout) Tries to obtain a lock with a name given by the string str, using a timeout of timeout seconds. Returns 1 if the lock was obtained successfully, 0 if the attempt timed out."));
    FUNC("@inet_aton"),    FT_MISC,    RET_NUMERIC,   PAR_STRING,  "INET_ATON(expr) Given the dotted-quad representation of an IPv4 network address as a string, returns an integer that represents the numeric value of the address in network byte order (big endian)."));
    FUNC("@inet_ntoa"),    FT_MISC,    RET_STRING,    PAR_NUMERIC,"INET_NTOA(expr) Given a numeric IPv4 network address in network byte order, returns the dotted-quad representation of the address as a binary string."));
    FUNC("@is_free_lock"), FT_MISC,    RET_NUMERIC,   PAR_STRING,  "IS_FREE_LOCK(str) Checks whether the lock named str is free to use (that is, not locked)."));
    FUNC("@is_used_lock"), FT_MISC,    RET_NUMERIC,   PAR_STRING,  "IS_USED_LOCK(str) Checks whether the lock named str is in use (that is, locked)."));
    FUNC("@master_pos_wait"), FT_MISC, RET_NUMERIC,   PAR_STRING, PAR_NUMERIC, OPAR_NUMERIC, "MASTER_POS_WAIT(log_name,log_pos[,timeout]) This function is useful for control of master/slave synchronization. It blocks until the slave has read and applied all updates up to the specified position in the master log. The return value is the number of log events the slave had to wait for to advance to the specified position."));
    FUNC("@name_const"),   FT_MISC,    RET_NUMERIC,   PAR_STRING,  PAR_STRING, "NAME_CONST(name,value) Returns the given value. When used to produce a result set column, NAME_CONST() causes the column to have the given name. The arguments should be constants."));
    FUNC("@release_lock"), FT_MISC,    RET_NUMERIC,   PAR_STRING,  "RELEASE_LOCK(str) Releases the lock named by the string str that was obtained with GET_LOCK()."));
    FUNC("@sleep"),        FT_MISC,    RET_NUMERIC,   PAR_NUMERIC, "SLEEP(duration) Sleeps (pauses) for the number of seconds given by the duration argument, then returns 0. If SLEEP() is interrupted, it returns 1."));
    FUNC("@uuid"),         FT_MISC,    RET_STRING,                 "UUID()Returns a Universal Unique Identifier (UUID) generated according to “DCE 1.1: Remote Procedure Call” (Appendix A) CAE (Common Applications Environment) Specifications published by The Open Group in October 1997 (Document Number C706, http://www.opengroup.org/public/pubs/catalog/c706.htm). "));


    return result;
}

const DatabaseBuiltinFunction& MySQLDatabaseEngine::getBuiltinFunction(const QString& name)
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

bool MySQLDatabaseEngine::tryConnect(Connection* c)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);
    bool ok = dbo.isOpen();

    if(!ok)
    {
        lastError = dbo.lastError().driverText() + "/" + dbo.lastError().databaseText();
        return false;
    }
    dbo.close();
    return true;

}

QStringList MySQLDatabaseEngine::getKeywords() const
{
    QStringList keywordPatterns;
    keywordPatterns
            <<"ADD"
            <<"ALL"
            <<"ALTER"
            <<"ANALYZE"
            <<"AND"
            <<"AS"
            <<"ASC"
            <<"ASENSITIVE"
            <<"BEFORE"
            <<"BETWEEN"
            <<"BOTH"
            <<"BY"
            <<"CALL"
            <<"CASCADE"
            <<"CASE"
            <<"CHANGE"
            <<"CHECK"
            <<"CHARACTER"
            <<"COLLATE"
            <<"COLUMN"
            <<"CONDITION"
            <<"CONSTRAINT"
            <<"CONTINUE"
            <<"CONVERT"
            <<"CREATE"
            <<"CROSS"
            <<"CURRENT_DATE"
            <<"CURRENT_TIME"
            <<"CURRENT_TIMESTAMP"
            <<"CURRENT_USER"
            <<"CURSOR"
            <<"DATABASE"
            <<"DATABASES"
            <<"DAY_HOUR"
            <<"DAY_MICROSECOND"
            <<"DAY_MINUTE"
            <<"DAY_SECOND"
            <<"DECLARE"
            <<"DEFAULT"
            <<"DELAYED"
            <<"DELETE"
            <<"DESC"
            <<"DESCRIBE"
            <<"DETERMINISTIC"
            <<"DISTINCT"
            <<"DISTINCTROW"
            <<"DIV"
            <<"DROP"
            <<"DUAL"
            <<"EACH"
            <<"ELSE"
            <<"ELSEIF"
            <<"ENCLOSED"
            <<"ESCAPED"
            <<"EXISTS"
            <<"EXIT"
            <<"EXPLAIN"
            <<"FALSE"
            <<"FETCH"
            <<"FOR"
            <<"FORCE"
            <<"FOREIGN"
            <<"FROM"
            <<"FULLTEXT"
            <<"GRANT"
            <<"GROUP"
            <<"HAVING"
            <<"HIGH_PRIORITY"
            <<"HOUR_MICROSECOND"
            <<"HOUR_MINUTE"
            <<"HOUR_SECOND"
            <<"IF"
            <<"IGNORE"
            <<"IN"
            <<"INDEX"
            <<"INFILE"
            <<"INNER"
            <<"INOUT"
            <<"INSENSITIVE"
            <<"INSERT"
            <<"INTERVAL"
            <<"INTO"
            <<"IS"
            <<"ITERATE"
            <<"JOIN"
            <<"KEY"
            <<"KEYS"
            <<"KILL"
            <<"LEADING"
            <<"LEAVE"
            <<"LEFT"
            <<"LIKE"
            <<"LIMIT"
            <<"LINES"
            <<"LOAD"
            <<"LOCALTIME"
            <<"LOCALTIMESTAMP"
            <<"LOCK"
            <<"LONG"
            <<"LONGBLOB"
            <<"LONGTEXT"
            <<"LOOP"
            <<"LOW_PRIORITY"
            <<"MATCH"
            <<"MEDIUMBLOB"
            <<"MEDIUMINT"
            <<"MEDIUMTEXT"
            <<"MIDDLEINT"
            <<"MINUTE_MICROSECOND"
            <<"MINUTE_SECOND"
            <<"MOD"
            <<"MODIFIES"
            <<"NATURAL"
            <<"NOT"
            <<"NO_WRITE_TO_BINLOG"
            <<"NULL"
            <<"ON"
            <<"OPTIMIZE"
            <<"OPTION"
            <<"OPTIONALLY"
            <<"OR"
            <<"ORDER"
            <<"OUT"
            <<"OUTER"
            <<"OUTFILE"
            <<"PRECISION"
            <<"PRIMARY"
            <<"PROCEDURE"
            <<"PURGE"
            <<"READ"
            <<"READS"
            <<"REAL"
            <<"REFERENCES"
            <<"REGEXP"
            <<"RELEASE"
            <<"RENAME"
            <<"REPEAT"
            <<"REPLACE"
            <<"REQUIRE"
            <<"RESTRICT"
            <<"RETURN"
            <<"REVOKE"
            <<"RIGHT"
            <<"RLIKE"
            <<"SCHEMA"
            <<"SCHEMAS"
            <<"SECOND_MICROSECOND"
            <<"SELECT"
            <<"SENSITIVE"
            <<"SEPARATOR"
            <<"SET"
            <<"SHOW"
            <<"SONAME"
            <<"SPATIAL"
            <<"SPECIFIC"
            <<"SQL"
            <<"SQLEXCEPTION"
            <<"SQLSTATE"
            <<"SQLWARNING"
            <<"SQL_BIG_RESULT"
            <<"SQL_CALC_FOUND_ROWS"
            <<"SQL_SMALL_RESULT"
            <<"SSL"
            <<"STARTING"
            <<"STRAIGHT_JOIN"
            <<"TABLE"
            <<"TERMINATED"
            <<"THEN"
            <<"TO"
            <<"TRAILING"
            <<"TRIGGER"
            <<"TRUE"
            <<"UNDO"
            <<"UNION"
            <<"UNIQUE"
            <<"UNLOCK"
            <<"UNSIGNED"
            <<"UPDATE"
            <<"USAGE"
            <<"USE"
            <<"USING"
            <<"UTC_DATE"
            <<"UTC_TIME"
            <<"UTC_TIMESTAMP"
            <<"VALUES"
            <<"VARYING"
            <<"WHEN"
            <<"WHERE"
            <<"WHILE"
            <<"WITH"
            <<"WRITE"
            <<"XOR"
            <<"YEAR_MONTH"
            <<"ZEROFILL"
            <<"ASENSITIVE"
            <<"CONNECTION"
            <<"DECLARE"
            <<"ELSEIF"
            <<"GOTO"
            <<"ITERATE"
            <<"LOOP"
            <<"READS"
            <<"RETURN"
            <<"SENSITIVE"
            <<"SQLEXCEPTION"
            <<"TRIGGER"
            <<"WHILE"
            <<"CALL"
            <<"CONTINUE"
            <<"DETERMINISTIC"
            <<"EXIT"
            <<"INOUT"
            <<"LABEL"
            <<"MODIFIES"
            <<"RELEASE"
            <<"SCHEMA"
            <<"SPECIFIC"
            <<"SQLSTATE"
            <<"UNDO"
            <<"CONDITION"
            <<"CURSOR"
            <<"EACH"
            <<"FETCH"
            <<"INSENSITIVE"
            <<"LEAVE"
            <<"OUT"
            <<"REPEAT"
            <<"SCHEMAS"
            <<"VIEW"
            <<"SORT"
            <<"SQL"
            <<"SQLWARNING"
            <<"UPGRADE"
            <<"DELIMITER"
            <<"BEGIN"
            <<"END";
    return keywordPatterns;
}


QStringList MySQLDatabaseEngine::getIndexTypes()
{
    return m_indexTypes;
}

QString MySQLDatabaseEngine::getDefaultIndextype()
{
    return m_defaultIndexType;
}

QString MySQLDatabaseEngine::getDelimiterKeyword()
{
    return "delimiter";
}

QVector<Codepage*> MySQLDatabaseEngine::getCodepages()
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

QStringList MySQLDatabaseEngine::getTriggerEvents()
{
    QStringList result;
    result << "INSERT" << "UPDATE" << "DELETE";
    return result;
}

QStringList MySQLDatabaseEngine::getTriggerTimings()
{
    QStringList result;
    result << "BEFORE" << "AFTER";
    return result;
}

Trigger* MySQLDatabaseEngine::reverseEngineerTrigger(Connection *c, const QString& procName)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

    bool ok = dbo.isOpen();

    if(!ok)
    {
        lastError = dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText();
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
            result = new Trigger(trigName);
            result->setEvent(event);
            result->setTable(table);
            result->setSql(stmt);
            result->setTime(timing);
        }
    }
    dbo.close();
    return result;
}
