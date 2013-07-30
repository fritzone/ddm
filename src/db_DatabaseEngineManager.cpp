#include "db_DatabaseEngineManager.h"
#include <QSqlDatabase>
#include <QDebug>

#include "dbmysql_DatabaseEngine.h"
#include "dbsqlite_DatabaseEngine.h"

#include "db_GenericDatabaseType.h"
#include "db_GenericDTSupplier.h"

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
    if(m_supportedEngines.contains(strQCUBRID))
    {
        m_supportedEngines << strCUBRID;
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

void DatabaseEngineManager::constructDtSupplier(const QString& dbName, const QVector<GenericDatabaseType*> dbtypes)
{
    GenericDTSupplier* gdts = new GenericDTSupplier(dbtypes);
    addDtSupplier(dbName.toUpper(), gdts);
}

void DatabaseEngineManager::setKeywords(const QString &dbName, const QStringList &keywords)
{
    m_keywords[dbName.toUpper()] = keywords;
}

QStringList DatabaseEngineManager::getKeywords(const QString &dbName)
{
    if(m_keywords.contains(dbName.toUpper()))
    {
        return m_keywords[dbName.toUpper()];
    }

    return QStringList();
}

void DatabaseEngineManager::setDelimiterKeyword(const QString &dbName, const QString &keyword)
{
    m_delimiterKeywords[dbName.toUpper()] = keyword;
}

QString DatabaseEngineManager::getDelimiterKeyword(const QString &dbName)
{
    if(m_delimiterKeywords.contains(dbName.toUpper()))
    {
        return m_delimiterKeywords[dbName.toUpper()];
    }

    return "";
}

void DatabaseEngineManager::setTriggerEvents(const QString &dbName, const QStringList &triggerEvents)
{
    m_triggerEvents[dbName.toUpper()] = triggerEvents;
}

QStringList DatabaseEngineManager::getTriggerEvents(const QString &dbName)
{
    if(m_triggerEvents.contains(dbName.toUpper()))
    {
        return m_triggerEvents[dbName.toUpper()];
    }

    return QStringList();
}

void DatabaseEngineManager::setTriggerTimes(const QString &dbName, const QStringList &triggerTimes)
{
    m_triggerTimes[dbName.toUpper()] = triggerTimes;
}

QStringList DatabaseEngineManager::getTriggerTimes(const QString &dbName)
{
    if(m_triggerTimes.contains(dbName.toUpper()))
    {
        return m_triggerTimes[dbName.toUpper()];
    }

    return QStringList();
}

void DatabaseEngineManager::setSps(const QString& dbName, const QVector<Sp*> sps, const QMap<QString, QString> sqls, const QMap<QString, QString> tooltips)
{
    m_sps[dbName.toUpper()] = sps;
    m_spSqlCommands[dbName.toUpper()] = sqls;
    m_spTooltips[dbName.toUpper()] = tooltips;
}
