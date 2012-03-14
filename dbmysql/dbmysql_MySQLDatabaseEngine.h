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
    virtual ~MySQLDatabaseEngine();

    virtual bool supportsEngines();
    virtual bool executeSql(Connection* c, const QStringList& sqls, QString& lastSql, bool rollbackOnError);
    virtual QString getDefaultDatatypesLocation();
    virtual bool reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, const QStringList& procs, const QStringList& funcs, const QStringList& triggers, Project* p, bool relaxed);
    virtual Table* reverseEngineerTable(Connection *c, const QString& tableName, Project* p, bool relaxed);
    virtual View* reverseEngineerView(Connection *c, const QString& viewName);
    virtual QStringList getAvailableDatabases(const QString& host, const QString& user, const QString& pass);
    virtual QStringList getAvailableTables(Connection* c);
    virtual QStringList getAvailableViews(Connection* c);
    virtual QStringList getAvailableStoredProcedures(Connection* c);
    virtual QStringList getAvailableStoredFunctions(Connection* c);
    virtual QStringList getAvailableTriggers(Connection* c);
    virtual bool createDatabase(Connection* c);
    virtual QVector<DatabaseBuiltinFunction> getBuiltinFunctions();
    virtual const DatabaseBuiltinFunction& getBuiltinFunction(const QString& name);
    virtual bool tryConnect(Connection* c);
    virtual QStringList getKeywords() const;
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c);
    virtual QStringList getColumnsOfTable(Connection* c, const QString& tableName);
    virtual bool dropDatabase(Connection* c);
    virtual QStringList getIndexTypes();
    virtual QString getDefaultIndextype();
    virtual QString getDelimiterKeyword();
    virtual QVector<Codepage*> getCodepages();
    virtual QStringList getTriggerEvents();
    virtual QStringList getTriggerTimings();
    virtual Procedure* reverseEngineerProc(Connection *c, const QString& procName);
    virtual Function* reverseEngineerFunc(Connection *c, const QString& funcName);
    virtual Trigger* reverseEngineerTrigger(Connection *c, const QString& procName);
    virtual QString getTableDescriptionScript(const QString& tabName);
    virtual QStringList getAvailableIndexes(Connection* c);
    virtual QString getTableCreationScript(Connection* c, const QString& tabName);
    virtual QVector<Sp*> getDatabaseSpecificProperties() const;
    virtual Sp* getSpForSqlRole(const QString& uid) const;

private:

    static QVector<DatabaseBuiltinFunction> buildFunctions();
    static QVector<Sp*> buildSps();
    static QString provideConnectionName(const QString&);

private:

    QMultiMap <UserDataType*, Column*> m_revEngMappings;
    QMap <QString, UserDataType*> m_oneTimeMappings;
    QStringList m_indexTypes;
    QString m_defaultIndexType;

private:

    static QVector<DatabaseBuiltinFunction>* s_builtinFunctions;
    static QVector<Sp*>* s_mysqlSpecificProperties;
    static int m_connectionCounter;
    static QMutex* m_connectionMutex;

};

#endif // MYSQLDATABASEENGINE_H
