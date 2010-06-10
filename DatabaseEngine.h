#ifndef DATABASEENGINE_H
#define DATABASEENGINE_H

#include <QMap>
#include <QString>

class AbstractDTSupplier;
class AbstractCodepageSupplier;
class AbstractIndextypeProvider;
class AbstractStorageEngineListProvider;

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

    const QString& getDatabase() const
    {
        return database;
    }

    static DatabaseEngine* createEngine(const QString& db);

    // anyone deriving from this class (such as MysqlDatabaseEngine) should provide this method to see if we show the combo box in the GUI
    virtual bool supportsEngines() = 0;

    virtual QString dbname() = 0;

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

    // whether the map above was initialized or not
    static bool genericInit;
};

#endif // DATABASEENGINE_H
