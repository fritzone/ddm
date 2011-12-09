#ifndef MYSQLDATABASEENGINE_H
#define MYSQLDATABASEENGINE_H

#include "db_DatabaseEngine.h"

#include <QSqlDatabase>
#include <QMutex>

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

    virtual bool injectSql(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QStringList& sqls, QString& lastSql, bool rollbackOnError, bool createTablesOnlyIfNotExist);
    virtual QString getDefaultDatatypesLocation();
    virtual bool reverseEngineerDatabase(const QString& host, const QString& user, const QString& pass, const QString& dbName, QVector<QString> tables, QVector<QString> views, Project* p, bool relaxed);
    virtual Table* reverseEngineerTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName, Project* p, bool relaxed);
    virtual View* reverseEngineerView(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& viewName);
    virtual QVector<QString> getAvailableDatabases(const QString& host, const QString& user, const QString& pass);
    virtual QVector<QString> getAvailableTables(const QString& host, const QString& user, const QString& pass, const QString& db);
    virtual QVector<QString> getAvailableViews(const QString& host, const QString& user, const QString& pass, const QString& db);
    virtual bool createDatabase(const QString& host, const QString& user, const QString& pass, const QString&);
    virtual QVector<DatabaseBuiltinFunction> getBuiltinFunctions();
    const DatabaseBuiltinFunction& getBuiltinFunction(const QString& name);
    virtual bool tryConnect(const QString& host, const QString& user, const QString& pass, const QString& dbName);
    virtual QStringList getKeywords() const;
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c);
    virtual QStringList getColumnsOfTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName);

private:

    static QVector<DatabaseBuiltinFunction> buildFunctions();
    static QString provideConnectionName(const QString&);

private:

    QMultiMap <UserDataType*, Column*> m_revEngMappings;
    QMap <QString, UserDataType*> m_oneTimeMappings;

    static QVector<DatabaseBuiltinFunction>* s_builtinFunctions;
    static QSqlDatabase* m_defaultMysqlDb;
    static int m_connectionCounter;
    static QMutex* m_connectionMutex;

};

#endif // MYSQLDATABASEENGINE_H
