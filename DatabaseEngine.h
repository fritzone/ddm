#ifndef DATABASEENGINE_H
#define DATABASEENGINE_H

#include <QMap>
#include <QString>

class AbstractDTSupplier;
class AbstractCodepageSupplier;
class AbstractIndextypeProvider;
class AbstractStorageEngineListProvider;
class AbstractSqlGenerator;
class Version;
class Table;
class Project;


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

    AbstractStorageEngineListProvider* getStorageEngineListProviders() const;

    /**
     * Returns an SQL generator for this database engine
     */
    AbstractSqlGenerator* getSqlGenerator() const;

    const QString& getDatabase() const
    {
        return database;
    }

    static DatabaseEngine* createEngine(const QString& db);

    // anyone deriving from this class (such as MysqlDatabaseEngine) should provide this method to see if we show the combo box in the GUI
    virtual bool supportsEngines() = 0;

    virtual QString dbname() = 0;

    virtual QString getDefaultDatatypesLocation() = 0;

    virtual bool injectSql(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QStringList& sqls, QString& lastSql, bool rollbackOnError, bool createTablesOnlyIfNotExist) = 0;

    /**
     * Reverse engineers the tables from the given database and puts the resulting structures in the v
     */
    virtual bool reverseEngineerDatabase(const QString& host, const QString& user, const QString& pass, const QString& dbName, QVector<QString> tables, Project* p) = 0;

    QString getLastError() const
    {
        return lastError;
    }

    virtual Table* reverseEngineerTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName, Project* p) = 0;

    virtual QVector<QString> getAvailableDatabases(const QString& host, const QString& user, const QString& pass) = 0;
    virtual QVector<QString> getAvailableTables(const QString& host, const QString& user, const QString& pass, const QString& db) = 0;
    QString getTypeStringForSqlType(const QString& sqlType);

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
