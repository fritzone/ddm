#include "db_DatabaseEngine.h"
#include "db_AbstractCodepageSupplier.h"
#include "dbmysql_MySQLDTSupplier.h"
#include "dbmysql_MySQLCodepageSupplier.h"
#include "dbmysql_MySQLIndextypeProvider.h"
#include "dbmysql_MySQLDatabaseEngine.h"
#include "dbmysql_MySQLStorageEngineListProvider.h"
#include "dbmysql_MySQLSQLGenerator.h"

QMap<QString, AbstractDTSupplier*> DatabaseEngine::dtsuppliers;
QMap<QString, AbstractCodepageSupplier*> DatabaseEngine::cpsuppliers;
QMap<QString, AbstractIndextypeProvider*> DatabaseEngine::indextypeProviders;
QMap<QString, AbstractStorageEngineListProvider*> DatabaseEngine::storageEngineProviders;
QMap<QString, AbstractSqlGenerator*> DatabaseEngine::sqlGenerators;

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
        sqlGenerators.insert(db, new MySQLSQLGenerator());
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

AbstractSqlGenerator* DatabaseEngine::getSqlGenerator() const
{
    return sqlGenerators.contains(database)?sqlGenerators[database]:0;
}

QString DatabaseEngine::getTypeStringForSqlType(const QString& sqlType)
{
    return getDTSupplier()->typeForSqlType(sqlType);
}
