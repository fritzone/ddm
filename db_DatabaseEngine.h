#ifndef DATABASEENGINE_H
#define DATABASEENGINE_H

#include <QMap>
#include <QString>
#include <QSqlDatabase>

class AbstractDTSupplier;
class AbstractCodepageSupplier;
class AbstractIndextypeProvider;
class AbstractStorageEngineListProvider;
class AbstractSqlGenerator;
class Version;
class Table;
class Project;
class DatabaseBuiltinFunction;
class View;
class Connection;

// BIG TODO: when a new database engine will be introduced which actually can work on databases
// redesign this interfece so that it takes in a Connection object instead of the host/user/pass
// combination since not every database has this feature. Sqlite for example

/**
 * This class is a kind of "wrapper" for the other classes that all deal
 * with specific database issues, such as Data Type Suppliers or
 * Codepage suppliers
 */
class DatabaseEngine
{
public:

    /**
     * Constructor. Gets in the Database type (MySQL, Oracle, ...)
     */
    DatabaseEngine(const QString& db);

    /**
     * Destructor
     */
    virtual ~DatabaseEngine()
    {
        //TODO: Add some deinitialization code here ...
    }

    /**
     * Returns the DataTypeSupplier of the specific DB
     */
    AbstractDTSupplier* getDTSupplier() const;

    /**
     * Returns the Codepage supplier of the engine
     */
    AbstractCodepageSupplier* getCodepageSupplier() const;

    /**
     * Returns the index types provided by this Database Engine
     */
    AbstractIndextypeProvider* getIndextypeProvider() const;

    /**
     * If the database supports storage engines, such as InnoDB for
     * MySQL this method returns an object that provides the Storage Engines to the outside world
     */
    AbstractStorageEngineListProvider* getStorageEngineListProviders() const;

    /**
     * Returns an SQL generator for this database engine
     */
    AbstractSqlGenerator* getSqlGenerator() const;

    /**
     * Returns the database name
     */
    const QString& getDatabase() const;

    // anyone deriving from this class (such as MysqlDatabaseEngine) should provide this method to see if we show the combo box in the GUI
    virtual bool supportsEngines() = 0;

    virtual QString getDefaultDatatypesLocation() = 0;

    virtual QStringList getKeywords() const = 0;


    virtual bool injectSql(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QStringList& sqls, QString& lastSql, bool rollbackOnError, bool createTablesOnlyIfNotExist) = 0;

    /**
     * Reverse engineers the tables from the given database and puts the resulting structures in the v
     */
    virtual bool reverseEngineerDatabase(const QString& host, const QString& user, const QString& pass, const QString& dbName,
                                         QVector<QString> tables, QVector<QString> views,
                                         Project* p, bool relaxed) = 0;

    QString getLastError() const
    {
        return lastError;
    }

    virtual Table* reverseEngineerTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName, Project* p, bool relaxed) = 0;

    virtual QVector<QString> getAvailableDatabases(const QString& host, const QString& user, const QString& pass) = 0;
    virtual QVector<QString> getAvailableTables(const QString& host, const QString& user, const QString& pass, const QString& db) = 0;
    virtual QVector<QString> getAvailableViews(const QString& host, const QString& user, const QString& pass, const QString& db) = 0;
    virtual bool createDatabase(const QString& host, const QString& user, const QString& pass, const QString&) = 0;
    virtual QVector<DatabaseBuiltinFunction> getBuiltinFunctions() = 0;
    virtual const DatabaseBuiltinFunction& getBuiltinFunction(const QString& name) = 0;
    virtual View* reverseEngineerView(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& viewName) = 0;

    QString getTypeStringForSqlType(const QString& sqlType);
    virtual bool tryConnect(const QString& host, const QString& user, const QString& pass, const QString& dbName) = 0;
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c) = 0;
    virtual QStringList getColumnsOfTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName) = 0;

public:

    /**
     * Creates a database engine for the given name. Right now "MySQL" is supported
     */
    static DatabaseEngine* createEngine(const QString& db);


private:

    // the database on which this is running
    QString database;

    // the list of data type suppliers supported
    static QMap<QString, AbstractDTSupplier*> dtsuppliers;

    // map of the Codepage suppliers supported
    static QMap<QString, AbstractCodepageSupplier*> cpsuppliers;

    // map of the index type providers
    static QMap<QString, AbstractIndextypeProvider*> indextypeProviders;

    static QMap<QString, AbstractStorageEngineListProvider*> storageEngineProviders;

    static QMap<QString, AbstractSqlGenerator*> sqlGenerators;

    // whether the maps above were initialized or not
    static bool genericInit;

protected:

    QString lastError;
};

#endif // DATABASEENGINE_H
