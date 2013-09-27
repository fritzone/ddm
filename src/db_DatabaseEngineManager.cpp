#include "db_DatabaseEngineManager.h"
#include <QSqlDatabase>
#include <QDebug>

#include "dbmysql_DatabaseEngine.h"
#include "dbsqlite_DatabaseEngine.h"

#include "dbsqlite_SQLGenerator.h"
#include "dbmysql_SQLGenerator.h"

#include "dbcubrid_DatabaseEngine.h"
#include "dbcubrid_SQLGenerator.h"

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
            //qDebug() << "Strange ...";
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
    //if(m_supportedEngines.contains(strQMySql) || m_supportedEngines.contains(strQMySql3))
    {
        m_supportedEngines << strMySql;
        MySQLDatabaseEngine* mysqlDBEngine = MySQLDatabaseEngine::instance();
        MySQLSQLGenerator* mysqlGenerator = new MySQLSQLGenerator(mysqlDBEngine);

        addEngine(strMySql, mysqlDBEngine);
        addSqlGenerator(strMySql, mysqlGenerator);
    }

    // is there sqlite driver?
    //if(m_supportedEngines.contains(strQSqlite))
    {
        m_supportedEngines << strSqlite;
        SqliteDatabaseEngine* sqliteDBEngine = SqliteDatabaseEngine::instance();
        SqliteSQLGenerator* sqliteGenrator = new SqliteSQLGenerator(sqliteDBEngine );

        addEngine(strSqlite, sqliteDBEngine);
        addSqlGenerator(strSqlite, sqliteGenrator);
    }

    // is there CUBRID driver?
    //if(m_supportedEngines.contains(strQCUBRID))
    {
        m_supportedEngines << strCUBRID;
        CUBRIDDatabaseEngine* cubridDbEngine = CUBRIDDatabaseEngine::instance();
        CUBRIDSQLGenerator* cubridGenerator = new CUBRIDSQLGenerator(cubridDbEngine);

        addEngine(strCUBRID, cubridDbEngine);
        addSqlGenerator(strCUBRID, cubridGenerator);
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

QMap<int, ReverseEngineerWizard::collectOperation> DatabaseEngineManager::getCollectOperations(const QString &name)
{
    static QMap<QString, QMap<int, ReverseEngineerWizard::collectOperation> > ops;
    static bool inited = false;
    if(!inited)
    {
        inited = true;

        QMap<int, ReverseEngineerWizard::collectOperation> opSqlite;
        opSqlite[1] = &ReverseEngineerWizard::gatherConnectionDataAndTablesStep;
        opSqlite[2] = &ReverseEngineerWizard::connectAndRetrieveViews;
        opSqlite[3] = &ReverseEngineerWizard::connectAndRetrieveTriggers;
        ops[strSqlite.toUpper()] = opSqlite;

        QMap<int, ReverseEngineerWizard::collectOperation> opMysql;
        opMysql[1] = &ReverseEngineerWizard::gatherConnectionDataAndCheckDatabase;
        opMysql[2] = &ReverseEngineerWizard::checkDatabaseAndCollectTables;
        opMysql[3] = &ReverseEngineerWizard::connectAndRetrieveViews;
        opMysql[4] = &ReverseEngineerWizard::connectAndRetrieveProcedures;
        opMysql[5] = &ReverseEngineerWizard::connectAndRetrieveFunctions;
        opMysql[6] = &ReverseEngineerWizard::connectAndRetrieveTriggers;
        ops[strMySql.toUpper()] = opMysql;

        QMap<int, ReverseEngineerWizard::collectOperation> opCUBRID;
        opCUBRID[1] = &ReverseEngineerWizard::gatherConnectionDataAndTablesStep;
        opCUBRID[2] = &ReverseEngineerWizard::connectAndRetrieveViews;
        opCUBRID[3] = &ReverseEngineerWizard::connectAndRetrieveProcedures;
        opCUBRID[4] = &ReverseEngineerWizard::connectAndRetrieveFunctions;
        opCUBRID[5] = &ReverseEngineerWizard::connectAndRetrieveTriggers;
        ops[strCUBRID.toUpper()] = opCUBRID;
    }
    return ops[name.toUpper()];
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

QVector<Sp *> DatabaseEngineManager::getSps(const QString &dbName)
{
    if(m_sps.contains(dbName.toUpper()))
    {
        return m_sps[dbName.toUpper()];
    }

    return QVector<Sp*>();
}

QString DatabaseEngineManager::getSpiExtensionSql(const QString &dbName, QUuid uid)
{
    if(m_spSqlCommands.contains(dbName.toUpper()))
    {
        if(m_spSqlCommands[dbName.toUpper()].contains(uid.toString()))
        {
            return m_spSqlCommands[dbName.toUpper()].value(uid.toString());
        }
    }

    qDebug() << "No SPI for" << uid.toString() << "in" << dbName;
    return "";
}

QString DatabaseEngineManager::getSpiExtensionTooltip(const QString &dbName, QUuid uid)
{
    if(m_spTooltips.contains(dbName.toUpper()))
    {
        if(m_spTooltips[dbName.toUpper()].contains(uid.toString()))
        {
            return m_spTooltips[dbName.toUpper()].value(uid.toString());
        }
    }

    return "";
}

void DatabaseEngineManager::setFunctionCategories(const QString &dbName, QVector<DatabaseFunctionCategory *> categories)
{
    m_functionCategories[dbName.toUpper()] = categories;
}

QVector<DatabaseFunctionCategory *> DatabaseEngineManager::getFunctionCategories(const QString &dbName)
{
    if(m_functionCategories.contains(dbName.toUpper()))
    {
        return m_functionCategories[dbName.toUpper()];
    }

    return QVector<DatabaseFunctionCategory*>();
}

void DatabaseEngineManager::setFunctions(const QString &dbName, const QVector<DatabaseBuiltinFunction> &funcs)
{
    m_dbFunctions[dbName.toUpper()] = funcs;
}

QVector<DatabaseBuiltinFunction> DatabaseEngineManager::getBuiltinFunctions(const QString &dbName)
{
    if(m_dbFunctions.contains(dbName.toUpper()))
    {
        return m_dbFunctions[dbName.toUpper()];
    }

    return QVector<DatabaseBuiltinFunction>();
}

void DatabaseEngineManager::setStoredMethodSupport(const QString &dbName, bool support)
{
    m_storedMethodsSupported[dbName.toUpper()] = support;
}

bool DatabaseEngineManager::getStoredMethodSupport(const QString &dbName)
{
    if(m_storedMethodsSupported.contains(dbName.toUpper()))
    {
        return m_storedMethodsSupported[dbName.toUpper()];
    }

    return false;
}

void DatabaseEngineManager::setStoredMethodSupportedLanguages(const QString &dbName, QSet<PROGRAMMING_LANGUAGES> languages)
{
    m_supportedLanguagesForStoredMethods[dbName.toUpper()] = languages;
}

QSet<PROGRAMMING_LANGUAGES> DatabaseEngineManager::getStoredMethodSupportedLanguages(const QString &dbName)
{
    if(m_supportedLanguagesForStoredMethods.contains(dbName.toUpper()))
    {
        return m_supportedLanguagesForStoredMethods[dbName.toUpper()];
    }

    return QSet<PROGRAMMING_LANGUAGES>();
}

void DatabaseEngineManager::setParameterFields(const QString &dbName, QMap<PARAMETER_FIELD_ROLES, int> fields)
{
    m_parameterFields[dbName.toUpper()] = fields;
}

QMap<PARAMETER_FIELD_ROLES, int> DatabaseEngineManager::getParameterFields(const QString &dbName)
{
    if(m_parameterFields.contains(dbName.toUpper()))
    {
        return m_parameterFields[dbName.toUpper()];
    }

    return QMap<PARAMETER_FIELD_ROLES, int>();
}

void DatabaseEngineManager::setStoredMethodDefaultBodies(const QString &dbName, QMap<PROGRAMMING_LANGUAGES, QString> bodies)
{
    m_storedMethodsDefaultBodies[dbName.toUpper()] = bodies;
}

QMap<PROGRAMMING_LANGUAGES, QString> DatabaseEngineManager::getStoredMethodDefaultBodies(const QString &dbName)
{
    if(m_storedMethodsDefaultBodies.contains(dbName.toUpper()))
    {
        return m_storedMethodsDefaultBodies[dbName.toUpper()];
    }

    return QMap<PROGRAMMING_LANGUAGES, QString>();
}

void DatabaseEngineManager::setStoredMethodReturnKeyword(const QString &dbName, QString keyword)
{
    m_returnKeywordForStoredFunction[dbName.toUpper()] = keyword;
}

QString DatabaseEngineManager::getStoredMethodReturnKeyword(const QString &dbName)
{
    if(m_returnKeywordForStoredFunction.contains(dbName.toUpper()))
    {
        return m_returnKeywordForStoredFunction[dbName.toUpper()];
    }

    return "";
}

void DatabaseEngineManager::setTriggerBodyDefinitionStatement(const QString &dbName, QString keyword)
{
    m_triggerStatements[dbName.toUpper()] = keyword;
}

QString DatabaseEngineManager::getTriggerBodyDefinitionStatement(const QString &dbName)
{
    if(m_triggerStatements.contains(dbName.toUpper()))
    {
        return m_triggerStatements[dbName.toUpper()];
    }

    return "";
}

void DatabaseEngineManager::setDBMenu(const QString &dbName, DBMenu* dbMenu)
{
    m_dbMenus[dbName.toUpper()] = dbMenu;
}

DBMenu* DatabaseEngineManager::getDBMenu(const QString &dbName)
{
    if(m_dbMenus.contains(dbName.toUpper()))
    {
        return m_dbMenus[dbName.toUpper()];
    }

    return 0;
}

void DatabaseEngineManager::setFkSupport(const QString &dbName, bool support)
{
    m_dbSupportsFks[dbName.toUpper()] = support;
}

bool DatabaseEngineManager::getFkSupport(const QString &dbName)
{
    if(m_dbMenus.contains(dbName.toUpper()))
    {
        return m_dbSupportsFks[dbName.toUpper()];
    }

    return false;
}

void DatabaseEngineManager::setFkSupportOnUpdate(const QString &dbName, QStringList support)
{
    m_dbFksOnUpdate[dbName.toUpper()] = support;
}

QStringList DatabaseEngineManager::getFkSupportOnUpdate(const QString &dbName)
{
    if(m_dbMenus.contains(dbName.toUpper()))
    {
        return m_dbFksOnUpdate[dbName.toUpper()];
    }

    return QStringList();
}

void DatabaseEngineManager::setFkSupportOnDelete(const QString &dbName, QStringList support)
{
    m_dbFksOnDelete[dbName.toUpper()] = support;
}

QStringList DatabaseEngineManager::getFkSupportOnDelete(const QString &dbName)
{
    if(m_dbMenus.contains(dbName.toUpper()))
    {
        return m_dbFksOnDelete[dbName.toUpper()];
    }

    return QStringList();
}

void DatabaseEngineManager::setDataQueries(const QString &dbName, QMap<QString, QSharedPointer<RepositoryQuery> > queries)
{
    m_dataQueries[dbName.toUpper()] = queries;
}

QSharedPointer<RepositoryQuery> DatabaseEngineManager::getQuery(const QString &dbName, const QString &queryId)
{
    if(m_dataQueries.contains(dbName.toUpper()))
    {
        if(m_dataQueries[dbName.toUpper()].contains(queryId.toUpper()))
        {
            return m_dataQueries[dbName.toUpper()][queryId.toUpper()];
        }
    }
    return QSharedPointer<RepositoryQuery>(0);
}
