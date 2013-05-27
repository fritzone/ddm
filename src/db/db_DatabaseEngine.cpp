#include "db_DatabaseEngine.h"
#include "dbmysql_MySQLDTSupplier.h"
#include "dbmysql_MySQLDatabaseEngine.h"
#include "dbmysql_MySQLSQLGenerator.h"
#include "dbsqlite_SqliteDatabaseEngine.h"

QMap<QString, AbstractDTSupplier*> DatabaseEngine::dtsuppliers;
QMap<QString, AbstractSqlGenerator*> DatabaseEngine::sqlGenerators;

bool DatabaseEngine::genericInit = DatabaseEngine::initAllEngines();

bool DatabaseEngine::initAllEngines()
{
    return false;
}

DatabaseEngine::DatabaseEngine(const QString& db, const QString& uid):
    Entity(db, uid, nullUid, false, false),
    database(db)
{
    if(!genericInit)
    {
        genericInit = true;
        // initialize the DT suppliers
        dtsuppliers.insert(db, new MySQLDTSupplier());

        // initialize the SQL Generators
        sqlGenerators.insert(db, new MySQLSQLGenerator(this));
    }
}

DatabaseEngine::~DatabaseEngine()
{
}

AbstractDTSupplier* DatabaseEngine::getDTSupplier() const
{
    return dtsuppliers.contains(database.toUpper())?dtsuppliers[database]:0;
}

DatabaseEngine* DatabaseEngine::provideEngineFor(const QString &db)
{
    if(db.toUpper() == "MYSQL") return MySQLDatabaseEngine::instance();
    if(db.toUpper() == "SQLITE") return SqliteDatabaseEngine::instance();
    return 0;
}

AbstractSqlGenerator* DatabaseEngine::getSqlGenerator() const
{
    return sqlGenerators.contains(database.toUpper())?sqlGenerators[database]:0;
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
