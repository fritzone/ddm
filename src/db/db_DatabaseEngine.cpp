#include "db_DatabaseEngine.h"
#include "dbmysql_MySQLDTSupplier.h"
#include "dbmysql_MySQLDatabaseEngine.h"
#include "dbmysql_MySQLSQLGenerator.h"

QMap<QString, AbstractDTSupplier*> DatabaseEngine::dtsuppliers;
QMap<QString, AbstractSqlGenerator*> DatabaseEngine::sqlGenerators;

bool DatabaseEngine::genericInit = false;

DatabaseEngine::DatabaseEngine(const QString& db, const QString& uid):
    Entity(db, uid, nullUid, false, false),
    database(db)
{
    if(!genericInit)
    {
        genericInit = true;
        // initialize the DT suppliers
        dtsuppliers.insert(db, new MySQLDTSupplier());
        sqlGenerators.insert(db, new MySQLSQLGenerator(this));
    }
}

DatabaseEngine::~DatabaseEngine()
{
}

AbstractDTSupplier* DatabaseEngine::getDTSupplier() const
{
    return dtsuppliers.contains(database)?dtsuppliers[database]:0;
}

DatabaseEngine* DatabaseEngine::provideEngineFor(const QString &db)
{
    if(db.toUpper() == "MYSQL") return MySQLDatabaseEngine::instance();
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

QString DatabaseEngine::getDatabaseEngineName() const
{
    return database;
}

QString DatabaseEngine::getLastError() const
{
    return lastError;
}
