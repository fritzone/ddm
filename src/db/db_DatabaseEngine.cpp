#include "db_DatabaseEngine.h"
#include "db_DatabaseEngineManager.h"
#include "uids.h"
#include "db_AbstractDTSupplier.h"

DatabaseEngine::DatabaseEngine(const QString& db, const QString& uid):
    Entity(db, uid, nullUid, false, false),
    database(db)
{
}

DatabaseEngine::~DatabaseEngine()
{
}

AbstractDTSupplier* DatabaseEngine::getDTSupplier() const
{
    return DatabaseEngineManager::instance().dtSupplier(database);
}

AbstractSqlGenerator* DatabaseEngine::getSqlGenerator() const
{
    return DatabaseEngineManager::instance().sqlGenerator(database);
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
