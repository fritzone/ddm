#ifndef DB_DATABASEENGINEMANAGER_H
#define DB_DATABASEENGINEMANAGER_H

#include <QStringList>
#include <QMap>

class AbstractDTSupplier;
class AbstractSqlGenerator;
class DatabaseEngine;

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

private:
    DatabaseEngineManager();

private:
    static DatabaseEngineManager* m_instance;

private:

    QStringList m_supportedEngines;

    // the list of data type suppliers supported
    QMap<QString, AbstractDTSupplier*> m_dtSuppliers;

    QMap<QString, AbstractSqlGenerator*> m_sqlGenerators;

    QMap<QString, DatabaseEngine*> m_dbEngines;


};

#endif // DB_DATABASEENGINEMANAGER_H
