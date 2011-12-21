#ifndef DATABASEENGINE_H
#define DATABASEENGINE_H

#include <QMap>
#include <QString>
#include <QSqlDatabase>

class AbstractDTSupplier;
class AbstractCodepageSupplier;
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
     * Constructor. Gets in the Database type as string (MySQL, Oracle, ...)
     */
    DatabaseEngine(const QString& db);

    /**
     * Destructor
     */
    virtual ~DatabaseEngine();

    /**
     * Returns the DataTypeSupplier of the specific DB.
     * @return the data supplier of this database engine
     */
    AbstractDTSupplier* getDTSupplier() const;

    /**
     * Returns the Codepage supplier of the engine
     * @return the code page supplier of this database engine
     */
    AbstractCodepageSupplier* getCodepageSupplier() const;

    /**
     * If the database supports storage engines, such as InnoDB for
     * MySQL this method returns an object that provides the Storage Engines to the outside world.
     * @return the storage engine list providers of this database
     */
    AbstractStorageEngineListProvider* getStorageEngineListProviders() const;

    /**
     * Returns an SQL generator for this database engine. The SQL generator is responsible for
     * generating SQLs for various SQL related jobs, such as table creation, or etc...
     * @return an SQL generator
     */
    AbstractSqlGenerator* getSqlGenerator() const;

    /**
     * Returns the database name for textual identification.
     * @return the name of the database (MySQL, Oracle, etc...)
     */
    const QString& getDatabaseEngineName() const;

    /**
     * Tells the name of the class of the DDM data type which provides access to this SQL type.
     * @return the DataType name providin access to the given SQL
     */
    QString getTypeStringForSqlType(const QString& sqlType);

    /**
     * Returns the last error of the database
     * @return the last error of the database
     */
    QString getLastError() const;

public:

    /*
     *
     * Pure virtual function declarations below
     *
     */

    /**
     * Tells the GUI if this database engine supports the "storage engine" feature (such as InnoDB for MySQL).
     * Anyone deriving from this class (such as MysqlDatabaseEngine) should provide this method to see if we
     * show the combo box in the GUI.
     * @return true if the DB supports storage engine, false if not
     */
    virtual bool supportsEngines() = 0;

    /**
     * On project creation there is a possibility to create a set of default database specific data types
     * to make the startup of a project easier. Each database has its own set of startup types,
     * they are stored in a project file and loaded when needed.
     * @return the location of the file storing the default data types
     */
    virtual QString getDefaultDatatypesLocation() = 0;

    /**
     * Returns a list of specific keywords for this database
     * @return the keywords of the database
     */
    virtual QStringList getKeywords() const = 0;

    /**
     * Executes the given SQL on the database represented by the connection object. On error sets the lasSql to be the last executed SQL.
     * @param conn - the connection into which the given SQLs will be inserted
     * @param sql - a list of SQLs that will be inserted
     * @param lastSql - the SQL that was executed before an error occured. In case of no error, the last SQL in the list.
     * @param rollbackOnError - in case of an error the code will try to execute a rollback if this is true.
     * @return true in case of success, false in case of error
     */
    virtual bool executeSql(Connection* conn, const QStringList& sqls, QString& lastSql, bool rollbackOnError) = 0;

    /**
     * Reverse engineers the tables and the views from the given connection and puts the resulting structures in the project passed in as a parameter
     * @param c - the connection representing the database
     * @param tables - the tables that should be reverse engineered
     * @param views - the views to reverse engineer
     * @param p - the project which will contain the reverse egnineered structures
     * @param relaxedDtCreation - instructs the engine on how to create data types for the column types. If it's false it tries to build data types
     * based on the name ofthe columns (allowing each column to have its own datatype, thus enforcing more strict foreign key policies) otherwise
     * it builds the data types from the SQL type, allowing more relaxed foreign keys.
     */
    virtual bool reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, Project* p, bool relaxedDtCreation) = 0;

    /**
     * Retrieves the list of index types that are supported on this specific database
     * @return the index types of the database
     */
    virtual QStringList getIndexTypes() = 0;

    /**
     * Return the default index type
     * @return the default index type
     */
    virtual QString getDefaultIndextype() = 0;

    /**
     * Reverse engineer a table.
     * @param c - the connection representing the database
     * @param tableName - the table to be reversed
     * @param p - the project which will get the new table (specifically its working version)
     * @param relaxedDtCreation - @see reverseEngineerDatabase
     * @return the reverse engineered table
     */
    virtual Table* reverseEngineerTable(Connection *c, const QString& tableName, Project* p, bool relaxedDtCreation) = 0;

    /**
     * Returns the databases which are availale on the given host, when connecting with the given credentials
     * @param host - the host
     * @param user - the user
     * @param pass - the password
     * @return a list of database names
     */
    virtual QStringList getAvailableDatabases(const QString& host, const QString& user, const QString& pass) = 0;

    virtual QStringList getAvailableTables(Connection* c) = 0;
    virtual QStringList getAvailableViews(Connection* c) = 0;
    virtual bool createDatabase(Connection* c) = 0;
    virtual QVector<DatabaseBuiltinFunction> getBuiltinFunctions() = 0;
    virtual bool dropDatabase(Connection* c) = 0;
    virtual const DatabaseBuiltinFunction& getBuiltinFunction(const QString& name) = 0;
    virtual View* reverseEngineerView(Connection *c, const QString& viewName) = 0;


    virtual bool tryConnect(const QString& host, const QString& user, const QString& pass, const QString& dbName) = 0;
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c) = 0;
    virtual QStringList getColumnsOfTable(Connection* c, const QString& tableName) = 0;

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

    static QMap<QString, AbstractStorageEngineListProvider*> storageEngineProviders;

    static QMap<QString, AbstractSqlGenerator*> sqlGenerators;

    // whether the maps above were initialized or not
    static bool genericInit;

protected:

    QString lastError;
};

#endif // DATABASEENGINE_H
