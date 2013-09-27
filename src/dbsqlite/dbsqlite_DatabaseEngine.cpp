#include "dbsqlite_DatabaseEngine.h"

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
#include "SqliteConnection.h"
#include "db_DatabaseEngineManager.h"

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

SqliteDatabaseEngine::SqliteDatabaseEngine() : DefaultDatabaseEngine("Sqlite", uidSqliteDb),
    m_revEngMappings(), m_oneTimeMappings()
{
    m_connectionMutex = new QMutex();
}

SqliteDatabaseEngine::~SqliteDatabaseEngine()
{
    delete m_connectionMutex;
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

        QStringList foundTables;

        for(int i=0; i<tables.size(); i++)
        {
            Table* tab = reverseEngineerTable(c, tables.at(i), p, relaxed, v);
            if(tab)
            {
                v->addTable(tab, true);
                foundTables.append(tab->getName());
            }
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
        for(int i=0; i<foundTables.size(); i++)
        {
            QSqlDatabase dbo = getQSqlDatabaseForConnection(c);
            bool ok = dbo.isOpen();

            if(ok)
            {
                QString s = QString("pragma foreign_key_list(") + foundTables[i] + ")";
                QSqlQuery query(dbo);
                query.exec(s);
                bool foundAtLeastOneForeignKey = false;
                while(query.next())
                {
                    QString id = query.value(0).toString();
                    QString seq = query.value(1).toString();
                    QString toTabName = query.value(2).toString();
                    QString fromColName = query.value(3).toString();
                    QString toColName = query.value(4).toString();
                    QString onUpdate = query.value(5).toString();
                    QString onDelete = query.value(6).toString();

                    QString referenceeTableName = foundTables[i];
                    QString referenceeColumnName = fromColName;

                    QString referencedTableName = toTabName;
                    QString referencedColumnName = toColName;
                    QString name = QString("fk_") + referencedTableName + "_"
                            + referencedColumnName;

                    ForeignKey* fk = createForeignKey(foundAtLeastOneForeignKey,
                                     referenceeTableName, v,
                                     referencedColumnName,
                                     referenceeColumnName,
                                     referencedTableName, name);
                    fk->setOnDelete(onDelete);
                    fk->setOnUpdate(onUpdate);

                }

                if(!foundAtLeastOneForeignKey)
                {
                    v->setSpecialValidationFlags(1);
                }

                dbo.close();
            }
        }
    }
    catch(...)
    {
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
        //qDebug() << dbo.lastError();
    }

    return dbo;
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
            udt = v->provideDatatypeForSqlType(field_name, type, bnullable, defaultValue, relaxed, getDTSupplier());
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

//TODO: Who on earth wrote this code? a 2 years old? FIX FIX FIX
void SqliteDatabaseEngine::parseTriggerSql(const QString &inSql, QString &outEvent, QString &outTime, QString &sql)
{
    QString currentWord = "";
    int i = 0;
    //qDebug() << inSql;
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
