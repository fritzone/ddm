#include "dbmysql_DatabaseEngine.h"

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
#include "core_Index.h"
#include "core_TableInstance.h"
#include "core_ForeignKey.h"
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
#include "MySqlConnection.h"
#include "db_DatabaseEngineManager.h"

int MySQLDatabaseEngine::m_connectionCounter = 1;
QMutex* MySQLDatabaseEngine::m_connectionMutex = 0;
MySQLDatabaseEngine* MySQLDatabaseEngine::s_instance = 0;

MySQLDatabaseEngine* MySQLDatabaseEngine::instance()
{
    if(s_instance == 0)
    {
        s_instance = new MySQLDatabaseEngine();
    }

    return s_instance;
}

QUuid MySQLDatabaseEngine::getClassUid() const
{
    return QUuid(m_classUid);
}

MySQLDatabaseEngine::MySQLDatabaseEngine() : DefaultDatabaseEngine(strMySql, uidMysqlDb),
    m_revEngMappings(), m_oneTimeMappings()
{
    m_connectionMutex = new QMutex();
}

MySQLDatabaseEngine::~MySQLDatabaseEngine()
{
    delete m_connectionMutex;
}

// this should be thread safe
QString MySQLDatabaseEngine::provideConnectionName(const QString& prefix)
{
    QMutexLocker lock(m_connectionMutex);
    QString t = prefix + QString::number(m_connectionCounter++);
    lock.unlock();
    return t;
}

bool MySQLDatabaseEngine::reverseEngineerDatabase(Connection *conn, const QStringList& tables, const QStringList& views, const QStringList& procs,
                                                  const QStringList& funcs, const QStringList& triggers, Project*p, bool relaxed)
{
    MySqlConnection* c = dynamic_cast<MySqlConnection*>(conn);
    if(!c)
    {
        return false;
    }

    try
    {
        Version* v = p->getWorkingVersion();
        m_revEngMappings.clear();
        m_oneTimeMappings.clear();

        for(int i=0; i<procs.size(); i++)
        {
            Procedure* proc = reverseEngineerProc(c, procs.at(i), v);
            if(proc)
            {
                v->addProcedure(proc, true);
            }
        }

        for(int i=0; i<funcs.size(); i++)
        {
            Function* func = reverseEngineerFunc(c, funcs.at(i), v);
            if(func)
            {
                v->addFunction(func, true);
            }
        }

        for(int i=0; i<tables.size(); i++)
        {
            Table* tab = reverseEngineerTable(c, tables.at(i), p, relaxed, v);
            if(tab)
            {
                v->addTable(tab, true);
            }
        }

        for(int i=0; i<views.size(); i++)
        {
            View* view = reverseEngineerView(c, views.at(i), v);
            if(view)
            {
                v->addView(view, true);
            }
        }

        for(int i=0; i<triggers.size(); i++)
        {
            Trigger* t= reverseEngineerTrigger(c, triggers.at(i), v);
            if(t)
            {
                v->addTrigger(t, true);
            }
        }

        // now populate the foreign keys
        QSqlDatabase dbo = getQSqlDatabaseForConnection(c);
        if(dbo.isOpen())
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

                ForeignKey* fk = createForeignKey(foundAtLeastOneForeignKey,
                                                  referenceeTableName, v,
                                                  referencedColumnName,
                                                  referenceeColumnName,
                                                  referencedTableName, name);

                Q_UNUSED(fk);
            }

            if(!foundAtLeastOneForeignKey)
            {
                v->setSpecialValidationFlags(1);
            }

            dbo.close();
        }

    }
    catch(...)
    {
        return false;
    }

    return true;
}

QSqlDatabase MySQLDatabaseEngine::getQSqlDatabaseForConnection(Connection *conn)
{
    MySqlConnection* c = dynamic_cast<MySqlConnection*>(conn);
    if(!c)
    {
        return QSqlDatabase::database();
    }

    QString newConnName = provideConnectionName(strConnection);
    QSqlDatabase dbo = QSqlDatabase::addDatabase(strQMySql, newConnName);

    dbo.setHostName(c->getHost());
    dbo.setDatabaseName(c->getDb());
    if(c->getPort()) dbo.setPort(c->getPort());
    dbo.open(c->getUser(), c->getPassword());

    return dbo;
}

Procedure* MySQLDatabaseEngine::reverseEngineerProc(Connection *c, const QString &procName, Version *v)
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
        proc = new Procedure(procName, QUuid::createUuid().toString(), v, false);
        proc->setSql(sql);
    }

    db.close();

    return proc;
}

Function* MySQLDatabaseEngine::reverseEngineerFunc(Connection *c, const QString &funcName, Version *v)
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
        func = new Function(funcName, QUuid::createUuid().toString(), v, false);
        func->setSql(sql);
    }

    db.close();

    return func;
}

View* MySQLDatabaseEngine::reverseEngineerView(Connection* conn, const QString& viewName, Version *v)
{
    MySqlConnection* c = dynamic_cast<MySqlConnection*>(conn);
    if(!c)
    {
        return 0;
    }

    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);

    if(!dbo.isOpen())
    {
        lastError = formatLastError(QObject::tr("Cannot reverse engineer a view"), dbo.lastError());
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
        view = new View(true, QUuid::createUuid().toString(), v);
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
    int fieldType = query.record().indexOf("Type");

    while(query.next())
    {
        QString field_name = query.value(fieldNo).toString();
        QString field_type = query.value(fieldType).toString();
        result.append(field_name + "@" + field_type);
    }
    return result;
}

Table* MySQLDatabaseEngine::reverseEngineerTable(Connection *conn, const QString& tableName, Project* p, bool relaxed, Version *ver)
{
    MySqlConnection* c = dynamic_cast<MySqlConnection*>(conn);
    if(!c)
    {
        return 0;
    }

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
    query.exec("desc " + tableName);

    int fieldNo = query.record().indexOf("Field");
    int typeNo = query.record().indexOf("Type");
    int nulldNo = query.record().indexOf("Null");
    int keyNo = query.record().indexOf("Key");
    int defNo = query.record().indexOf("Default");

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
            bool bnullable = QString::compare(nullable, "YES", Qt::CaseInsensitive) == 0;
            udt = v->provideDatatypeForSqlType(field_name, type, bnullable, defaultValue, relaxed, getDTSupplier());
            m_oneTimeMappings.insert(oneTimeKey, udt);
        }
        Column* col = new Column(QUuid::createUuid().toString(), field_name, udt, QString::compare(keyness, "PRI", Qt::CaseInsensitive) == 0, ver);

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
        QString order = "DESC";
        if(collation == "A") order = "ASC";
        if(collation.toUpper() == "NULL") order = "";
        if(createdIndexes.keys().contains(keyname))
        {
            idx = createdIndexes[keyname];
        }
        else
        {
            idx = new Index(finalIndexName, tab, QUuid::createUuid().toString(), ver);
            // TODO: set the index type SPI based on indextype from above
            createdIndexes.insert(keyname, idx);
        }

        idx->addColumn(tab->getColumn(columnname), order, seqinindex.toInt());
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
            // and create a table instance for it, no need for new name, this is reverse engineering
            TableInstance* inst = v->instantiateTable(tab, false);
            inst->setName(tab->getName());
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

        {
        // see if we have a storage engine
        SpInstance* spi = tab->getInstanceForSqlRoleUid(this, uidMysqlStorageEngineTable);
        if(spi)
        {
            spi->set(eng);
        }
        }
    }
    dbo.close();
    return tab;
}



QStringList MySQLDatabaseEngine::getAvailableDatabases(const QString& host, const QString& user, const QString& pass, int port)
{
    Connection *c = new MySqlConnection("temp", host, user, pass, "", false, false, port);
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    bool ok = db.isOpen();

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot run a query"), db.lastError());

        return QStringList();
    }

    QStringList result;
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

bool MySQLDatabaseEngine::dropDatabase(Connection* conn)
{
    MySqlConnection* c = dynamic_cast<MySqlConnection*>(conn);
    if(!c)
    {
        return 0;
    }

    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    bool ok = db.isOpen();
    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot connect to the DB for dropping a database"), db.lastError());
        return false;
    }

    QSqlQuery query(db);
    bool t = query.exec("drop database "+ c->getDb());
    if(!t)
    {
        lastError = formatLastError(QObject::tr("Cannot drop database: ") + c->getDb(), db.lastError());
        return false;
    }
    db.close();
    c->setState(DROPPED);
    return true;
}

bool MySQLDatabaseEngine::createDatabase(Connection* conn)
{
    MySqlConnection* c = dynamic_cast<MySqlConnection*>(conn);
    if(!c)
    {
        return 0;
    }

    MySqlConnection *c1 = new MySqlConnection("temp", c->getHost(), c->getUser(), c->getPassword(), "", false, false, c->getPort());
    QSqlDatabase db = getQSqlDatabaseForConnection(c1);
    bool ok = db.isOpen();

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot connect to the DB server to create a new database"), db.lastError());
        return false;
    }

    QSqlQuery query(db);
    bool t = query.exec("create database "+ c->getDb());
    if(!t)
    {
        lastError = formatLastError(QObject::tr("Cannot create database: ") + c->getDb(), query.lastError());
        return false;
    }
    db.close();
    return true;
}

bool MySQLDatabaseEngine::tryConnect(Connection* c)
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

Trigger* MySQLDatabaseEngine::reverseEngineerTrigger(Connection *c, const QString& procName, Version *v)
{
    QSqlDatabase dbo = getQSqlDatabaseForConnection(c);
    if(!dbo.isOpen())
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

QString MySQLDatabaseEngine::getTableDescriptionScript(const QString& tabName)
{
    QString result = "desc " + tabName;
    return result;
}

QStringList MySQLDatabaseEngine::getAvailableIndexes(Connection* conn)
{
    MySqlConnection* c = dynamic_cast<MySqlConnection*>(conn);
    if(!c)
    {
        return QStringList();
    }

    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    QStringList result;

    bool ok = db.isOpen();

    if(!ok)
    {
        return result;
    }

    QSqlQuery query(db);
    query.exec("SELECT DISTINCT TABLE_NAME, INDEX_NAME, COLUMN_NAME FROM INFORMATION_SCHEMA.STATISTICS WHERE TABLE_SCHEMA = '" + c->getDb() + "'");

    int tabNameIdx = query.record().indexOf("TABLE_NAME");
    int indexNameIdx = query.record().indexOf("INDEX_NAME");
    int columnNameIdx = query.record().indexOf("COLUMN_NAME");

    while(query.next())
    {
        QString tabName = query.value(tabNameIdx).toString();
        QString indexName = query.value(indexNameIdx).toString();
        QString columnName = query.value(columnNameIdx).toString();

        result.append(tabName + "." + indexName + " (" + columnName +")");
    }
    db.close();
    return result;
}

QString MySQLDatabaseEngine::getViewCreationScript(Connection* c, const QString& tabName)
{
    return getTableCreationScript(c, tabName);
}

QString MySQLDatabaseEngine::getTableCreationScript(Connection* c, const QString& tabName)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    QString result;

    bool ok = db.isOpen();

    if(!ok)
    {
        return result;
    }

    {
    QSqlQuery query(db);
    query.exec("SET sql_mode = 'ANSI'");
    }

    QSqlQuery query(db);
    query.exec("show create table " + tabName);

    while(query.next())
    {
        result = query.value(1).toString();
    }
    db.close();
    return result;
}

bool MySQLDatabaseEngine::tableBlocksForeignKeyFunctionality(const Table* table) const
{
    SpInstance* spi = table->getInstanceForSqlRoleUid(this, uidMysqlStorageEngineTable);
    if(spi)
    {
        QString storageEngine = spi->get();
        if(storageEngine.length())
        {
            if(storageEngine.toUpper() == "INNODB") return false;
        }
    }
    return true;
}

QStringList MySQLDatabaseEngine::getSupportedStorageEngines(const QString& host, const QString& user, const QString& pass, int port)
{
    MySqlConnection *c = new MySqlConnection("temp", host, user, pass, "", false, false, port);
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    bool ok = db.isOpen();
    QStringList result;

    if(!ok)
    {
        lastError = formatLastError(QObject::tr("Cannot connect to the database"), db.lastError());
        return result;
    }

    QSqlQuery query(db);
    query.exec("show engines");

    while(query.next())
    {
        QString dbName = query.value(0).toString();
        bool supported = query.value(1).toString().toUpper() != "NO";
        if(supported) result.append(dbName);
    }
    db.close();

    return result;
}
