#include "DatabaseEngine.h"
#include "AbstractCodepageSupplier.h"
#include "MySQLDTSupplier.h"
#include "MysqlCodepageSupplier.h"
#include "MySQLIndextypeProvider.h"
#include "MySQLDatabaseEngine.h"
#include "MySQLStorageEngineListProvider.h"

QMap<QString, AbstractDTSupplier*> DatabaseEngine::dtsuppliers;
QMap<QString, AbstractCodepageSupplier*> DatabaseEngine::cpsuppliers;
QMap<QString, AbstractIndextypeProvider*> DatabaseEngine::indextypeProviders;
QMap<QString, AbstractStorageEngineListProvider*> DatabaseEngine::storageEngineProviders;

bool DatabaseEngine::genericInit = false;

DatabaseEngine::DatabaseEngine(const QString& db):database(db)
{
    if(!genericInit)
    {
        genericInit = true;
        // initialize the DT suppliers
        dtsuppliers.insert(db, new MySQLDTSupplier());
        cpsuppliers.insert(db, new MySQLCodepageSupplier());
        indextypeProviders.insert(db, new MySQLIndexTypeProvider());
        storageEngineProviders.insert(db, new MySQLStorageEngineListProvider());
    }

}

AbstractDTSupplier* DatabaseEngine::getDTSupplier() const
{
    return dtsuppliers.contains(database)?dtsuppliers[database]:0;
}

AbstractCodepageSupplier* DatabaseEngine::getCodepageSupplier() const
{
    return cpsuppliers.contains(database)?cpsuppliers[database]:0;
}

AbstractIndextypeProvider* DatabaseEngine::getIndextypeProvider() const
{
    return indextypeProviders.contains(database)?indextypeProviders[database]:0;
}

AbstractStorageEngineListProvider* DatabaseEngine::getStorageEngineListProviders() const
{
    return storageEngineProviders.contains(database)?storageEngineProviders[database]:0;
}

DatabaseEngine* DatabaseEngine::createEngine(const QString &db)
{
    if(db == "MySQL") return new MySQLDatabaseEngine();
    return 0;
}
