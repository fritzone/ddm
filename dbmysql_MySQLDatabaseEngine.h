#ifndef MYSQLDATABASEENGINE_H
#define MYSQLDATABASEENGINE_H

#include "db_DatabaseEngine.h"

#include <QSqlDatabase>
#include <QMutex>
#include <QStringList>

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

    virtual bool executeSql(Connection* conn, const QStringList& sqls, QString& lastSql, bool rollbackOnError);
    virtual QString getDefaultDatatypesLocation();
    virtual bool reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, Project* p, bool relaxed);
    virtual Table* reverseEngineerTable(Connection *c, const QString& tableName, Project* p, bool relaxed);
    virtual View* reverseEngineerView(Connection *c, const QString& viewName);
    virtual QStringList getAvailableDatabases(const QString& host, const QString& user, const QString& pass);
    virtual QStringList getAvailableTables(Connection* c);
    virtual QStringList getAvailableViews(Connection* c);
    virtual bool createDatabase(Connection* c);
    virtual QVector<DatabaseBuiltinFunction> getBuiltinFunctions();
    const DatabaseBuiltinFunction& getBuiltinFunction(const QString& name);
    virtual bool tryConnect(Connection* c);
    virtual QStringList getKeywords() const;
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c);
    virtual QStringList getColumnsOfTable(Connection* c, const QString& tableName);
    virtual bool dropDatabase(Connection* c);
    virtual QStringList getIndexTypes();
    virtual QString getDefaultIndextype();
    virtual QString getDelimiterKeyword();
    virtual QVector<Codepage*> getCodepages();

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
    QStringList m_indexTypes;
    QString m_defaultIndexType;

};

#endif // MYSQLDATABASEENGINE_H
