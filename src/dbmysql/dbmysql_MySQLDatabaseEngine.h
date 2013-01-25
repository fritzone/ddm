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

    static MySQLDatabaseEngine* instance();

public:

    virtual ~MySQLDatabaseEngine();

    virtual QUuid getClassUid() const;

    virtual bool executeSql(Connection* c, const QStringList& sqls, const QStringList& uid, QString& lastSql, bool rollbackOnError);
    virtual QString getDefaultDatatypesLocation();
    virtual bool reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, const QStringList& procs, const QStringList& funcs, const QStringList& triggers, Project* p, bool relaxed);
    virtual Table* reverseEngineerTable(Connection *c, const QString& tableName, Project* p, bool relaxed, Version* ver);
    virtual View* reverseEngineerView(Connection *c, const QString& viewName, Version* v);
    virtual QStringList getAvailableDatabases(const QString& host, const QString& user, const QString& pass, int port);
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
    virtual Procedure* reverseEngineerProc(Connection *c, const QString& procName, Version *v);
    virtual Function* reverseEngineerFunc(Connection *c, const QString& funcName, Version* v);
    virtual Trigger* reverseEngineerTrigger(Connection *c, const QString& procName, Version* v);
    virtual QString getTableDescriptionScript(const QString& tabName);
    virtual QStringList getAvailableIndexes(Connection* c);
    virtual QString getTableCreationScript(Connection* c, const QString& tabName);
    virtual QVector<Sp*> getDatabaseSpecificProperties() const;
    virtual Sp* getSpForSqlRole(const QString& uid) const;
    virtual bool tableBlocksForeignKeyFunctionality(const Table* table) const;
    virtual bool injectMetadata(Connection* c, const Version* v);
    virtual QString getDbMetadata(Connection *c);

    QStringList getSupportedStorageEngines(const QString& host, const QString& user, const QString& pass, int port);
    QString formatLastError(const QString &header, const QSqlError&);

private:
    MySQLDatabaseEngine();

    static QVector<DatabaseBuiltinFunction> buildFunctions();
    static QVector<Sp*> buildSps();
    static QString provideConnectionName(const QString&);
    static QStringList getCodepageList();
    QString toHexString(const QString& x);
    QStringList chopUpString(const QString& x, int size);

private:

    QMultiMap <UserDataType*, Column*> m_revEngMappings;
    QMap <QString, UserDataType*> m_oneTimeMappings;
    QStringList m_indexTypes;
    QString m_defaultIndexType;

private:

    static MySQLDatabaseEngine* s_instance;
    static QVector<DatabaseBuiltinFunction>* s_builtinFunctions;
    static QVector<Sp*>* s_mysqlSpecificProperties;
    static int m_connectionCounter;
    static QMutex* m_connectionMutex;

};

#endif // MYSQLDATABASEENGINE_H
