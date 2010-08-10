#include "MySQLDatabaseEngine.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include <QMessageBox>

MySQLDatabaseEngine::MySQLDatabaseEngine() : DatabaseEngine("MySQL")
{
}

bool MySQLDatabaseEngine::injectSql(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QStringList& sqls)
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

    QSqlQuery query;

    for(int i=0; i<sqls.size(); i++)
    {
        if(!query.exec(sqls[i]))
        {
            lastError = query.lastError().databaseText() + "/" + query.lastError().databaseText();
            return false;
        }
    }

    return true;
}
