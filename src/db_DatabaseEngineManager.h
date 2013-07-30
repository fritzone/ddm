#ifndef DB_DATABASEENGINEMANAGER_H
#define DB_DATABASEENGINEMANAGER_H

#include <QStringList>
#include <QMap>
#include <QDomElement>

class AbstractDTSupplier;
class AbstractSqlGenerator;
class DatabaseEngine;
class GenericDatabaseType;
class Sp;

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
};

#endif // DB_DATABASEENGINEMANAGER_H
