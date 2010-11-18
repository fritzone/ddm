#ifndef MYSQLDATABASEENGINE_H
#define MYSQLDATABASEENGINE_H

#include "DatabaseEngine.h"

class MySQLDatabaseEngine : public DatabaseEngine
{
public:
    MySQLDatabaseEngine();

    virtual bool supportsEngines()
    {
        return true;
    }

    virtual QString dbname()
    {
        return "MySQL";
    }

    virtual bool injectSql(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QStringList& sqls, QString& lastSql, bool rollbackOnError, bool createTablesOnlyIfNotExist);
    virtual QString getDefaultDatatypesLocation();
    virtual bool reverseEngineerDatabase(const QString& host, const QString& user, const QString& pass, const QString& dbName, QVector<QString> tables, Version* v);
    virtual Table* reverseEngineerTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName, Version* v);
    virtual QVector<QString> getAvailableDatabases(const QString& host, const QString& user, const QString& pass);
    virtual QVector<QString> getAvailableTables(const QString& host, const QString& user, const QString& pass, const QString& db);

};

#endif // MYSQLDATABASEENGINE_H
