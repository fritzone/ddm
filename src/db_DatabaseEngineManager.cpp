#include "db_DatabaseEngineManager.h"
#include <QSqlDatabase>
#include <QDebug>

#include "dbmysql_MySQLDatabaseEngine.h"
#include "dbsqlite_SqliteDatabaseEngine.h"

#include "strings.h"

DatabaseEngineManager* DatabaseEngineManager::m_instance = 0;

DatabaseEngineManager& DatabaseEngineManager::instance()
{
    if(m_instance == 0)
    {
        static DatabaseEngineManager instance;
        if(m_instance != &instance)
        {
            qDebug() << "Strange ...";
        }

    }
    return *m_instance;
}

DatabaseEngineManager::DatabaseEngineManager()
{
    // need this here, the DB engines will call the instance
    m_instance = this;

    m_supportedEngines = QSqlDatabase::drivers();

    // is there mysql driver?
    if(m_supportedEngines.contains(strQMySql) || m_supportedEngines.contains(strQMySql3))
    {
        m_supportedEngines << strMySql;
        MySQLDatabaseEngine* mysqlDBEngine = MySQLDatabaseEngine::instance();
        mysqlDBEngine->setup();
    }

    // is there sqlite driver?
    if(m_supportedEngines.contains(strQSqlite))
    {
        m_supportedEngines << strSqlite;
        SqliteDatabaseEngine* sqliteDBEngine = SqliteDatabaseEngine::instance();
        sqliteDBEngine->setup();
    }

    // is there CUBRID driver?
    if(m_supportedEngines.contains(strQCubrid))
    {
        m_supportedEngines << strCubrid;
    }

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

bool DatabaseEngineManager::supportsEngine(const QString &name)
{
    QString upper = name.toUpper();
    if(m_sqlGenerators.contains(upper))
    {
        return true;
    }

    return false;
}


void DatabaseEngineManager::addDtSupplier(const QString &name, AbstractDTSupplier *supplier)
{
    m_dtSuppliers.insert(name, supplier);
}

void DatabaseEngineManager::addSqlGenerator(const QString &name, AbstractSqlGenerator *gen)
{
    m_sqlGenerators.insert(name, gen);
}
