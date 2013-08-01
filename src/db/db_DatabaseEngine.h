#ifndef DATABASEENGINE_H
#define DATABASEENGINE_H

#include "core_ObjectWithUid.h"
#include "core_Entity.h"

#include <QMap>
#include <QString>
#include <QSqlDatabase>
#include <QStringList>

class AbstractDTSupplier;
class AbstractSqlGenerator;
class Version;
class Table;
class Project;
class DatabaseBuiltinFunction;
class View;
class Connection;
class Codepage;
class Procedure;
class Function;
class Trigger;
class Sp;
class Column;


/**
 * The DatabaseEngine class is the common interface for accessing the database
 * the DDM works with in a uniform way. It exposes the most common methods
 * for operations the user can do on a database.
 */
class DatabaseEngine : public Entity
{
public:

    /**
     * Constructor. Gets in the Database name as string (MySQL, Oracle, ...)
     */
    DatabaseEngine(const QString& db, const QString &uid);

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
     * Returns an SQL generator for this database engine. The SQL generator is
     * responsible for generating SQLs for various SQL related jobs, such as
     * table creation, or etc...
     * @return an SQL generator
     */
    AbstractSqlGenerator* getSqlGenerator() const;

    /**
     * Returns the database name for textual identification.
     * @return the name of the database (MySQL, Oracle, etc...)
     */
    QString getDatabaseEngineName() const;

    /**
     * Tells the name of the class of the DDM data type which provides access to
     * this SQL type.
     * @return the DataType name providin access to the given SQL
     */
    QString getTypeStringForSqlType(const QString& sqlType);

    /**
     * Returns the last error of the database
     * @return the last error of the database
     */
    QString getLastError() const;

public:

    /**
     * @group
     * Pure virtual functions
     */

    /**
     * Returns a list of specific keywords for this database
     * @return the keywords of the database
     */
    virtual QStringList getKeywords() const = 0;

    /**
     * @group Generic database related functions such as connection, run an SQL, etc...
     */

    /**
     * On project creation there is a possibility to create a set of default
     * database specific data types to make the startup of a project easier.
     * Each database has its own set of startup types, they are stored in a
     * project file and loaded when needed.
     *
     * @return the location of the file storing the default data types
     */
    virtual QString getDefaultDatatypesLocation() = 0;

    /**
     * Executes the given SQL on the database represented by the connection
     * object. On error sets the lasSql to be the last executed SQL.
     *
     * @param conn - the connection into which the given SQLs will be inserted
     * @param sql - a list of SQLs that will be inserted
     * @param lastSql - the SQL that was executed before an error occured. In
     *        case of no error, the last SQL in the list.
     * @param rollbackOnError - in case of an error the code will try to
     *        execute a rollback if this is true.
     *
     * @return true in case of success, false in case of error
     */
    virtual bool executeSql(Connection* c, const QStringList& sqls,
                            const QStringList& uid,
                            QString& lastSql, bool rollbackOnError) = 0;

    /**
     * Tries to connect to the database represented by the connection
     * @param c - the connection
     * @return true in case of success, false otherwise
     */
    virtual bool tryConnect(Connection* c) = 0;

    /**
     * Returns a QSqlDatabase object for the given connection, which can be
     * used to execute own queries
     * @param c - the conenction
     * @return a QSqlDatabase obejct for the given connection
     */
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c) = 0;

    /**
     * @group Database syntax related functions
     */

    /**
     * Returns the delimiter keyword used in the function declaration
     * @return the delimiter keyword
     */
    virtual QString getDelimiterKeyword() = 0;

    /**
     * Returns the Events of the triggers that are supported by this DB engine
     *
     * @return the Events of the triggers that are supported by this DB engine
     */
    virtual QStringList getTriggerEvents() = 0;

    /**
     * Returns the Times of the triggers that are supported by this DB engine
     *
     * @return the Times of the triggers that are supported by this DB engine
     */
    virtual QStringList getTriggerTimings() = 0;

    /**
     * Returns a list of @see DatabaseBuiltingFunction objects that are
     * representing the functions supported by the database
     *
     * @return a list of teh functions of the database
     */
    virtual QVector<DatabaseBuiltinFunction> getBuiltinFunctions() = 0;

    /**
     * @group Database structure retrieval and reverse engineering functions
     */

    /**
     * Returns the databases which are availale on the given host,
     * when connecting with the given credentials
     *
     * @param host - the host
     * @param user - the user
     * @param pass - the password
     *
     * @return a list of database names
     */
    virtual QStringList getAvailableDatabases(const QString& host, const QString& user, const QString& pass, int port) = 0;

    /**
     * Returnd a list of the tables that are in the database represented by
     * the given connection
     *
     * @param c - the connection to the database
     *
     * @return the list of the tables in this connection
     */
    virtual QStringList getAvailableTables(Connection* c) = 0;

    /**
     * Returns a list of the available views that are in the database
     * represented by the given connection
     *
     * @param c - the connection to the database
     * @return the list of views
     */
    virtual QStringList getAvailableViews(Connection* c) = 0;

    /**
     * Returns a list of the available procedures that are in the database
     * represented by the given connection
     *
     * @param c - the connection to the database
     *
     * @return the list of procedure names
     */
    virtual QStringList getAvailableStoredProcedures(Connection* c) = 0;

    /**
     * Returns a list of the available functions that are in the database
     * represented by the given connection
     *
     * @param c - the connection to the database
     *
     * @return the list of function names
     */
    virtual QStringList getAvailableStoredFunctions(Connection* c) = 0;

    /**
     * Returns a list of the available triggers that are in the database
     * represented by the given connection
     *
     * @param c - the connection to the database
     *
     * @return the list of trigger names
     */
    virtual QStringList getAvailableTriggers(Connection* c) = 0;

    /**
     * Returns a list of the available indexes that are in the database
     * represented by the given connection
     *
     * @param c - the connection to the database
     *
     * @return the list of index names
     */
    virtual QStringList getAvailableIndexes(Connection* c) = 0;

    /**
     * Reverse engineer a table.
     *
     * @param c - the connection representing the database
     * @param tableName - the table to be reversed
     * @param p - the project which will get the new table (specifically its
     * working version)
     * @param relaxedDtCreation - @see reverseEngineerDatabase
     *
     * @return the reverse engineered table
     */
    virtual Table* reverseEngineerTable(Connection *c, const QString& tableName,
                                        Project* p, bool relaxedDtCreation, Version* v) = 0;

    /**
     * Executes a script which creates the database represented by the
     * connection. The database engine must support the notion
     * of separated databases.
     *
     * @param c - the connection that represents a database
     *
     * @return true in case of success, false otherwise
     */
    virtual bool createDatabase(Connection* c) = 0;

    /**
     * Executes a script which drops the database represented by the connection.
     * The database engine must support the notion
     * of separated databases.
     *
     * @param c - the connection that represents a database
     *
     * @return true in case of success, false otherwise
     */
    virtual bool dropDatabase(Connection* c) = 0;

    /**
     * Returns the representation of the database function called name
     *
     * @param name - the name of the function
     * @return the function, or if it does not exist a function with name
     *
     * "N/A" and type FT_INVALID
     */
    virtual const DatabaseBuiltinFunction& getBuiltinFunction(const QString& name) = 0;

    /**
     * Reverse engineers the view which is to be found in the database
     * represented by the connection object. The resulting view object will
     * always have the manual flag turned on, so there is no SQL query builder
     * for it.
     *
     * @param c - the connection
     * @param viewName - the view to be reverse engineered
     *
     * @return the View object representing the database view, or 0 in case
     *         there is no view.
     */
    virtual View* reverseEngineerView(Connection *c, const QString& viewName, Version* v) = 0;

    /**
     * Reverse engineers the functions which is to be found in the database
     * represented by the connection object.
     *
     * @param c - the connection
     * @param procName - the function to be reverse engineered
     *
     * @return the Function object representing the function in the database
     *         or 0 in case there is no function called like that
     */
    virtual Function* reverseEngineerFunc(Connection *c, const QString& funcName, Version* v) = 0;

    /**
     * Reverse engineers the procedure which is to be found in the database
     * represented by the connection object.
     *
     * @param c - the connection
     * @param procName - the procedure to be reverse engineered
     *
     * @return the Procedure object representing the procedure in the database
     *         or 0 in case there is no procedure called like that
     */
    virtual Procedure* reverseEngineerProc(Connection *c, const QString& procName, Version* v) = 0;

    /**
     * Reverse engineers the trigger which is to be found in the database
     * represented by the connection object.
     *
     * @param c - the connection
     * @param procName - the trigger to be reverse engineered
     * @return the Procedure object representing the procedure in the database
     *         or 0 in case there is no procedure called like that
     */
    virtual Trigger* reverseEngineerTrigger(Connection *c, const QString& procName, Version* v) = 0;

    /**
     * Returns the columns of the given table as a list of strings
     * @param c - the connection
     * @param  tableName - the name of the table
     * @return the list of the columns of the table
     */
    virtual QStringList getColumnsOfTable(Connection* c, const QString& tableName) = 0;

    /**
     * Reverse engineers the tables and the views from the given connection and
     * puts the resulting structures in the project passed in as a parameter
     * @param c - the connection representing the database
     * @param tables - the tables that should be reverse engineered
     * @param views - the views to reverse engineer
     * @param p - the project which will contain the reverse egnineered structures
     * @param relaxedDtCreation - instructs the engine on how to create data
     *        types for the column types. If it's false it tries to build data
     *        types based on the name ofthe columns (allowing each column to
     *        have its own datatype, thus enforcing more strict foreign key
     *        policies) otherwise it builds the data types from the SQL type,
     *        allowing more relaxed foreign keys.
     *
     * @return true in case of success, false if an error occured. You can
     *        call @see lastError to see the error
     */
    virtual bool reverseEngineerDatabase(Connection *c,
                                         const QStringList& tables,
                                         const QStringList& views,
                                         const QStringList& procs,
                                         const QStringList& funcs,
                                         const QStringList& triggers,
                                         Project* p, bool relaxedDtCreation) = 0;

    /**
     * Returns an SQL script for getting the properties of the given table.
     * @param tabName - the table to get the description for
     * @return an SQL script for getting the properties of the given table.
     */
    virtual QString getTableDescriptionScript(const QString& tabName) = 0;

    /**
     * Returns an SQL script for getting the creation script of the given table.
     * @param c - the connection which holds the table
     * @param tabName - the table to get the description for
     * @return an SQL script how to create the given table.
     */
    virtual QString getTableCreationScript(Connection* c, const QString& tabName) = 0;

    /**
     * @brief getViewCreationScript returns the creation script for the view
     * @param c
     * @param name
     * @return
     */
    virtual QString getViewCreationScript(Connection* c, const QString& name) = 0;

    /**
     * Returns a vector of Database Specific properties this database provides.
     * @return a vector of Database Specific properties this database provides.
     */
    virtual QVector<Sp*> getDatabaseSpecificProperties() const = 0;

    /**
     * Returns the SP of the given database which has the given SQL role uid.
     * @param uid - the uid
     * @return the SP for the uid
     */
    virtual Sp* getSpForSqlRole(const QString& uid) const = 0;

    /**
     * Checks if the deployment of this table will block the foreign keys.
     * Pretty useful for ex. MyISAM mysql engines.
     * @param table - the table to check
     * @return true if the table will block the foreign key functionality
     */
    virtual bool tableBlocksForeignKeyFunctionality(const Table* table) const = 0;

    /**
     * Injects the xml (representing the version metadata) into a special table
     * which acts as metadata holder in the database. The metadata is basically
     * the serialized data of the version. The table (as of 0.1j) contains
     * a timestamp and a clob field.
     *
     * @param v - the version whose metadata should be inserted
     * @param c - the connection
     *
     * @return true in case of success of false in case of failure
     */
    virtual bool injectMetadata(Connection* c, const Version* v) = 0;

    /**
     * Returns the metadata that was stored in a previous deployment in this
     * database. If there is no metadata it returns an empty string.
     *
     * @param c - the connection
     *
     * @return the metadata of the database or an empty string
     */
    virtual QString getDbMetadata(Connection *c) = 0;

    /**
     * @brief supportsStoredMethods returns true if the database supports stored functions
     * and procedures.
     */
    virtual bool supportsStoredMethods() = 0;

    /**
     * @brief spiExtension returns the SQL extension for the given SPI uid,
     * such as TEMPORARY for uidTemporaryTable. This will be a long and ugly
     * method, but will be better for the generalization of the SQL generation.
     * @return
     */
    virtual QString spiExtension(QUuid) = 0;

private:

    // the database on which this is running
    QString database;

protected:

    QString lastError;
};

#endif // DATABASEENGINE_H
