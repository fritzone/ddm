#ifndef MYSQLDATABASEENGINE_H
#define MYSQLDATABASEENGINE_H

#include "DatabaseEngine.h"

class UserDataType;
class Column;

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
    virtual bool reverseEngineerDatabase(const QString& host, const QString& user, const QString& pass, const QString& dbName, QVector<QString> tables, Project* p, bool relaxed);
    virtual Table* reverseEngineerTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName, Project* p, bool relaxed);
    virtual QVector<QString> getAvailableDatabases(const QString& host, const QString& user, const QString& pass);
    virtual QVector<QString> getAvailableTables(const QString& host, const QString& user, const QString& pass, const QString& db);


private:

    QMultiMap <UserDataType*, Column*> m_revEngMappings;
    QMap <QString, UserDataType*> m_oneTimeMappings;

};

#endif // MYSQLDATABASEENGINE_H
