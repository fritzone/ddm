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
