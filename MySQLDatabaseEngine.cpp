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

MySQLDatabaseEngine::MySQLDatabaseEngine() : DatabaseEngine("MySQL")
{
}

bool MySQLDatabaseEngine::reverseEngineerDatabase(const QString& host, const QString& user, const QString& pass, const QString& dbName, QVector<QString> tables, Version* v)
{
    for(int i=0; i<tables.size(); i++)
    {
        reverseEngineerTable(host, user, pass, dbName, tables.at(i), v);
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


Table* MySQLDatabaseEngine::reverseEngineerTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName, Version* v)
{
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

    QSqlQuery query;

    Table* tab = new Table(v);
    tab->setName(tableName);

    query.exec("desc " + tableName);

    int fieldNo = query.record().indexOf("Field");
    int typeNo = query.record().indexOf("Type");
    int nulldNo = query.record().indexOf("Null");
    int keyNo = query.record().indexOf("Key");
    int defNo = query.record().indexOf("Default");
    int extraNo = query.record().indexOf("Extra");

    // fetch all the columns of the table
    while(query.next())
    {
        QString field_name = query.value(fieldNo).toString();
        QString type = query.value(typeNo).toString().toUpper();
        QString nullable = query.value(nulldNo).toString();
        QString keyness = query.value(keyNo).toString();
        QString defaultValue = query.value(defNo).toString();
        QString extra = query.value(extraNo).toString();

        // now fetch the data type for the given type from the version
        UserDataType* udt = v->provideDatatypeForSqlType(type, nullable, defaultValue);
        Column* col = new Column(field_name, udt, QString::compare(keyness, "PRI", Qt::CaseInsensitive) == 0, extra == "auto_increment");
        tab->addColumn(col);
    }

    v->addTable(tab);

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
