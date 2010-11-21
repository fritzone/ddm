#include "MySQLDatabaseEngine.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>
#include <QMessageBox>

#include "Table.h"
#include "Version.h"
#include "Column.h"
#include "Project.h"
#include "VersionGuiElements.h" // TODO: This is bad design. Find a way to remove this from here
#include "UserDataType.h"
#include "Index.h"

MySQLDatabaseEngine::MySQLDatabaseEngine() : DatabaseEngine("MySQL"), m_revEngMappings(), m_oneTimeMappings()
{
}

bool MySQLDatabaseEngine::reverseEngineerDatabase(const QString& host, const QString& user, const QString& pass, const QString& dbName, QVector<QString> tables, Project*p, bool relaxed)
{
    Version* v = p->getWorkingVersion();
    m_revEngMappings.clear();
    m_oneTimeMappings.clear();
    for(int i=0; i<tables.size(); i++)
    {
        Table* tab = reverseEngineerTable(host, user, pass, dbName, tables.at(i), p, relaxed);
        v->addTable(tab);
        v->getGui()->createTableTreeEntry(tab);
    }

    // and now try to find some nice names for the user data types
    QList <UserDataType*> rkeys = m_revEngMappings.keys();
    for(int i=0; i<rkeys.size(); i++)
    {
        QList <Column*> colsOfUdt = m_revEngMappings.values(rkeys.at(i));
        for(int j=0; j<colsOfUdt.size(); j++)
        {
            qDebug() << "DATATYPE " << rkeys.at(i)->getName() << " AS COLUMN " << colsOfUdt.at(j)->getName();
        }
    }
    return true;
}


QVector<QString> MySQLDatabaseEngine::getAvailableTables(const QString& host, const QString& user, const QString& pass, const QString& db)
{
    QSqlDatabase dbo = QSqlDatabase::addDatabase("QMYSQL");

    dbo.setHostName(host);
    dbo.setUserName(user);
    dbo.setPassword(pass);
    dbo.setDatabaseName(db);

    QVector<QString> result;
    bool ok = dbo.open();

    if(!ok)
    {
        QMessageBox::critical(0, "Error", dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText(), QMessageBox::Ok);
        return result;
    }

    QSqlQuery query;

    query.exec("show tables");

    while(query.next())
    {
        QString tab = query.value(0).toString();
        result.append(tab);
    }

    return result;
}


Table* MySQLDatabaseEngine::reverseEngineerTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName, Project* p, bool relaxed)
{
    Version* v = p->getWorkingVersion();
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(pass);
    db.setDatabaseName(dbName);

    bool ok = db.open();

    if(!ok)
    {
        QMessageBox::critical(0, "Error", db.lastError().databaseText() + "/" + db.lastError().databaseText(), QMessageBox::Ok);
        return 0;
    }

    Table* tab = new Table(v);
    tab->setName(tableName);

    // fetch all the columns of the table
    {
    QSqlQuery query;
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
        if(m_oneTimeMappings.contains(oneTimeKey))
        {
            udt = m_oneTimeMappings.value(oneTimeKey);
        }
        else
        {
            udt = v->provideDatatypeForSqlType(field_name, type, nullable, defaultValue, relaxed);
            m_oneTimeMappings.insert(oneTimeKey, udt);
        }
        // now fetch the data type for the given type from the version

        Column* col = new Column(field_name, udt, QString::compare(keyness, "PRI", Qt::CaseInsensitive) == 0, extra == "auto_increment");

        // and add the column to the table
        m_revEngMappings.insert(udt, col);
        tab->addColumn(col);
    }
    }

    // now populate the indices of the table
    {

    QSqlQuery query;
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

    QVector<Index*> indexes;

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

        Index* idx = new Index(keyname, indextype, tab);

    }
    }

    return tab;
}

bool MySQLDatabaseEngine::injectSql(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QStringList& sqls, QString& lastSql, bool rollbackOnError, bool createTablesOnlyIfNotExist)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(pass);
    db.setDatabaseName(dbName);

    bool ok = db.open();


    if(!ok)
    {
        lastError = db.lastError().databaseText() + "/" + db.lastError().databaseText();
        return false;
    }

    bool transactionSucces = db.transaction();
    QSqlQuery query;

    for(int i=0; i<sqls.size(); i++)
    {
        lastSql = sqls[i].trimmed();
        if(lastSql.length() > 0)
        {
            if(!query.exec(lastSql))
            {
                lastError = query.lastError().databaseText() + "/" + query.lastError().databaseText();
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
    return true;
}

QString MySQLDatabaseEngine::getDefaultDatatypesLocation()
{
    return "mysql.defaults";
}

QVector<QString> MySQLDatabaseEngine::getAvailableDatabases(const QString& host, const QString& user, const QString& pass)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(pass);

    bool ok = db.open();
    QVector<QString> result;

    if(!ok)
    {
        lastError = QObject::tr("Cannot connect to the database: ") + db.lastError().databaseText() + "/" + db.lastError().driverText();
        return result;
    }

    QSqlQuery query;
    query.exec("show databases");

    while(query.next())
    {
        QString db = query.value(0).toString();
        result.append(db);
    }

    return result;
}
