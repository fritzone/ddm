#include "db_DatabaseEngineManager.h"
#include <QSqlDatabase>
#include <QDebug>

#include "dbmysql_MySQLDatabaseEngine.h"
#include "dbmysql_MySQLDTSupplier.h"
#include "dbmysql_MySQLDatabaseEngine.h"
#include "dbmysql_MySQLSQLGenerator.h"

#include "dbsqlite_SqliteDatabaseEngine.h"
#include "dbsqlite_SqliteDatabaseEngine.h"
#include "dbsqlite_SqliteDTSupplier.h"
#include "dbsqlite_SqliteSQLGenerator.h"

#include "strings.h"

DatabaseEngineManager* DatabaseEngineManager::m_instance = 0;

DatabaseEngineManager& DatabaseEngineManager::instance()
{
    if(m_instance == 0)
    {
        static DatabaseEngineManager instance;
        m_instance = &instance;
    }
    return *m_instance;
}

DatabaseEngineManager::DatabaseEngineManager()
{
    m_supportedEngines = QSqlDatabase::drivers();
    m_supportedEngines << strMySql << strSqlite;

    // initialize the database engines
    MySQLDatabaseEngine* mysqlDBEngine = MySQLDatabaseEngine::instance();
    m_dbEngines.insert(strMySql, mysqlDBEngine);
    m_dbEngines.insert(strQMySql, mysqlDBEngine);
    m_dbEngines.insert(strQMySql3, mysqlDBEngine);

    SqliteDatabaseEngine* sqliteDBEngine = SqliteDatabaseEngine::instance();
    m_dbEngines.insert(strSqlite, sqliteDBEngine);
    m_dbEngines.insert(strQSqlite, sqliteDBEngine);

    // initialize the DT suppliers
    MySQLDTSupplier *mysqlDtSupplier = new MySQLDTSupplier();

    m_dtSuppliers.insert(strMySql, mysqlDtSupplier);
    m_dtSuppliers.insert(strQMySql, mysqlDtSupplier);
    m_dtSuppliers.insert(strQMySql3, mysqlDtSupplier);

    SqliteDTSupplier* sqliteDtSupplier = new SqliteDTSupplier();
    m_dtSuppliers.insert(strQSqlite, sqliteDtSupplier);
    m_dtSuppliers.insert(strSqlite, sqliteDtSupplier);

    // initialize the SQL Generators
    MySQLSQLGenerator* mysqlGenerator = new MySQLSQLGenerator(mysqlDBEngine);

    m_sqlGenerators.insert(strMySql, mysqlGenerator);
    m_sqlGenerators.insert(strQMySql, mysqlGenerator);
    m_sqlGenerators.insert(strQMySql3, mysqlGenerator);

    SqliteSQLGenerator* sqliteGenrator = new SqliteSQLGenerator(sqliteDBEngine);
    m_sqlGenerators.insert(strSqlite, sqliteGenrator);
    m_sqlGenerators.insert(strQSqlite, sqliteGenrator);
}


DatabaseEngine* DatabaseEngineManager::engine(const QString &name)
{
    QString upper = name.toUpper();
    if(m_dbEngines.contains(upper))
    {
        return m_dbEngines.value(upper);
    }

    return 0;
}

AbstractDTSupplier* DatabaseEngineManager::dtSupplier(const QString &name)
{
    QString upper = name.toUpper();
    if(m_dtSuppliers.contains(upper))
    {
        return m_dtSuppliers.value(upper);
    }

    return 0;
}

AbstractSqlGenerator* DatabaseEngineManager::sqlGenerator(const QString &name)
{
    QString upper = name.toUpper();
    if(m_sqlGenerators.contains(upper))
    {
        return m_sqlGenerators.value(upper);
    }

    return 0;
}
