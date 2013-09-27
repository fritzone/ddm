#ifndef DB_DATABASEENGINEMANAGER_H
#define DB_DATABASEENGINEMANAGER_H

#include "db_DatabaseBuiltinFunction.h"

#include "reveng_ReverseEngineerWizard.h"
#include "db_RepositoryQuery.h"

#include <QStringList>
#include <QMap>
#include <QDomElement>
#include <QUuid>

class AbstractDTSupplier;
class AbstractSqlGenerator;
class DatabaseEngine;
class GenericDatabaseType;
class Sp;
class DatabaseFunctionCategory;
class DBMenu;


class DatabaseEngineManager
{
public:
    static DatabaseEngineManager& instance();

public:

    /**
     * @brief engine provides an engine for the given name
     * @param name
     * @return
     */
    DatabaseEngine* engine(const QString& name);

    /**
     * @brief dtSupplier returns the DT supplier for the given db
     * @param name
     * @return
     */
    AbstractDTSupplier* dtSupplier(const QString& name);

    /**
     * @brief sqlGenerator returns an SQL generator for the given db
     * @param name
     * @return
     */
    AbstractSqlGenerator* sqlGenerator(const QString& name);

    /**
     * @brief supportsEngine returns true if the given engine is supported
     * (ie: Qt sql plugin and driver were sucesfully loaded)
     * @param name
     * @return
     */
    bool supportsEngine(const QString& name);

    /**
     * @brief addEngine adds the given engine with the specific name. There
     * can be more names for one engine.
     * @param name
     * @param engine
     */
    template <class T>
    void addEngine(const QString& name, T* engine)
    {
        m_dbEngines.insert(name, engine);
    }

    /**
     * @brief addDtsupplier adds a data type supplier to the manager.
     * @param name
     * @param supplier
     */
    void addDtSupplier(const QString& name, AbstractDTSupplier* supplier);

    /**
     * @brief addSqlGenerator adds an SQL generator.
     * @param name
     * @param gen
     */
    void addSqlGenerator(const QString& name, AbstractSqlGenerator* gen);

    /**
     * @brief constructDtSupplier creates a new a DT supplier from the given XML
     * @param el
     */
    void constructDtSupplier(const QString &dbName, const QVector<GenericDatabaseType *> dbtypes);

    /**
     * @brief getKeywords returns the keywords of the given database
     * @param dbName
     * @return
     */
    QStringList getKeywords(const QString& dbName);

    /**
     * @brief setKeywords sets the keywords for the given database
     * @param dbName
     * @param keywords
     */
    void setKeywords(const QString& dbName, const QStringList& keywords);

    /**
     * @brief setDelimiterKeyword
     * @param dbName
     * @param keyword
     */
    void setDelimiterKeyword(const QString& dbName, const QString& keyword);

    /**
     * @brief getDelimiterKeyword returns the delimiter keyword for the given DB
     * @param dbName
     * @return
     */
    QString getDelimiterKeyword(const QString& dbName);

    QMap<int, ReverseEngineerWizard::collectOperation> getCollectOperations(const QString& name);


    /**
     * @brief setTriggerEvents
     * @param dbName
     * @param triggerEvents
     */
    void setTriggerEvents(const QString &dbName, const QStringList &triggerEvents);

    QStringList getTriggerEvents(const QString &dbName);

    void setTriggerTimes(const QString &dbName, const QStringList &triggerTimes);

    QStringList getTriggerTimes(const QString &dbName);

    void setSps(const QString& dbName, const QVector<Sp*> sps, const QMap<QString, QString> sqls, const QMap<QString, QString> tooltips);

    QVector<Sp*> getSps(const QString& dbName);

    QString getSpiExtensionSql(const QString& dbName, QUuid uid);

    QString getSpiExtensionTooltip(const QString& dbName, QUuid uid);

    void setFunctionCategories(const QString& dbName, QVector<DatabaseFunctionCategory*> categories);

    QVector<DatabaseFunctionCategory*> getFunctionCategories(const QString& dbName);

    void setFunctions(const QString& dbName, const QVector<DatabaseBuiltinFunction>& funcs);

    QVector<DatabaseBuiltinFunction> getBuiltinFunctions(const QString& dbName);

    void setStoredMethodSupport(const QString &dbName, bool support);

    bool getStoredMethodSupport(const QString &dbName);

    void setStoredMethodSupportedLanguages(const QString &dbName, QSet<PROGRAMMING_LANGUAGES> languages);

    QSet<PROGRAMMING_LANGUAGES> getStoredMethodSupportedLanguages(const QString &dbName);

    void setParameterFields(const QString &dbName, QMap<PARAMETER_FIELD_ROLES, int> fields);

    QMap<PARAMETER_FIELD_ROLES, int> getParameterFields(const QString &dbName);

    void setStoredMethodDefaultBodies(const QString &dbName, QMap<PROGRAMMING_LANGUAGES, QString> bodies);

    QMap<PROGRAMMING_LANGUAGES, QString> getStoredMethodDefaultBodies(const QString &dbName);

    void setStoredMethodReturnKeyword(const QString &dbName, QString keyword);

    QString getStoredMethodReturnKeyword(const QString &dbName);

    void setTriggerBodyDefinitionStatement(const QString &dbName, QString keyword);

    QString getTriggerBodyDefinitionStatement(const QString &dbName);

    void setDBMenu(const QString &dbName, DBMenu* dbMenu);

    DBMenu* getDBMenu(const QString &dbName);

    void setFkSupport(const QString &dbName, bool support);

    bool getFkSupport(const QString &dbName);

    void setFkSupportOnUpdate(const QString &dbName, QStringList support);

    QStringList getFkSupportOnUpdate(const QString &dbName);

    void setFkSupportOnDelete(const QString &dbName, QStringList support);

    QStringList getFkSupportOnDelete(const QString &dbName);

    void setDataQueries(const QString& dbName, QMap<QString, QSharedPointer<RepositoryQuery> > queries);

    QSharedPointer<RepositoryQuery> getQuery(const QString& dbName, const QString& queryId);

private:
    DatabaseEngineManager();

private:
    static DatabaseEngineManager* m_instance;

private:

    QStringList m_supportedEngines;

    // the list of data type suppliers supported
    QMap<QString, AbstractDTSupplier*> m_dtSuppliers;

    // the SQL generators
    QMap<QString, AbstractSqlGenerator*> m_sqlGenerators;

    // the DB engines
    QMap<QString, DatabaseEngine*> m_dbEngines;

    // stores the keywords, so that we don't have to keep them in the DB engines
    QMap<QString, QStringList> m_keywords;

    // the delimiter keywords for each database
    QMap<QString, QString> m_delimiterKeywords;

    // the map of trigger events per database
    QMap<QString, QStringList> m_triggerEvents;

    // the map of trigger times per database
    QMap<QString, QStringList> m_triggerTimes;

    // the SPs for each of the database
    QMap<QString, QVector<Sp*> > m_sps;

    // holds the tooltips of a given database (first key) for each SPs uid (second key)
    QMap<QString, QMap<QString, QString> > m_spTooltips;

    // holds the SQL command of a given database (first key) for each SPs uid (second key)
    QMap<QString, QMap<QString, QString> > m_spSqlCommands;

    // holds the function categories of a database. Used in the query builder dialog
    QMap<QString, QVector<DatabaseFunctionCategory*> > m_functionCategories;

    // holds the functions of a database as defined in the repo
    QMap<QString, QVector<DatabaseBuiltinFunction> > m_dbFunctions;

    // if the stored methods are supported for the given database
    QMap<QString, bool> m_storedMethodsSupported;

    // what language can be used to write sotred methods
    QMap<QString, QSet<PROGRAMMING_LANGUAGES> > m_supportedLanguagesForStoredMethods;

    // how to seamlessly define the parameters
    QMap<QString, QMap<PARAMETER_FIELD_ROLES, int> > m_parameterFields;

    // has the default bodies for the sotred methods of the specified language
    QMap<QString, QMap<PROGRAMMING_LANGUAGES, QString> > m_storedMethodsDefaultBodies;

    // the RETURN keyword for a specific database... It can be different :(
    QMap<QString, QString> m_returnKeywordForStoredFunction;

    // the trigger creation statement for a specific database
    QMap<QString, QString> m_triggerStatements;

    // the map containing the context menu of a connection item
    QMap<QString, DBMenu*> m_dbMenus;

    // if the database supports the foreign key feature
    QMap<QString, bool> m_dbSupportsFks;

    // the map containig the foreign key support for ON UPDATE referential action
    QMap<QString, QStringList> m_dbFksOnUpdate;

    // the map containig the foreign key support for ON DELETE referential action
    QMap<QString, QStringList> m_dbFksOnDelete;

    // the data queries of each of the databases
    QMap<QString, QMap<QString, QSharedPointer<RepositoryQuery> > > m_dataQueries;
};

#endif // DB_DATABASEENGINEMANAGER_H
