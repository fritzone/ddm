#ifndef SQLITEDATABASEENGINE_H
#define SQLITEDATABASEENGINE_H

#include "db_DefaultDatabaseEngine.h"

#include <QSqlDatabase>
#include <QMutex>
#include <QStringList>

class UserDataType;
class Column;

class SqliteDatabaseEngine : public DefaultDatabaseEngine
{
public:

    static SqliteDatabaseEngine* instance();

public:

    virtual ~SqliteDatabaseEngine();

    virtual QUuid getClassUid() const;
    virtual void setup();
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
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c);
    virtual QStringList getColumnsOfTable(Connection* c, const QString& tableName);
    virtual bool dropDatabase(Connection* c);
    virtual QString getDelimiterKeyword();
    virtual QStringList getTriggerEvents();
    virtual QStringList getTriggerTimings();
    virtual Procedure* reverseEngineerProc(Connection *c, const QString& procName, Version *v);
    virtual Function* reverseEngineerFunc(Connection *c, const QString& funcName, Version* v);
    virtual Trigger* reverseEngineerTrigger(Connection *c, const QString& procName, Version* v);
    virtual QString getTableDescriptionScript(const QString& tabName);
    virtual QStringList getAvailableIndexes(Connection* c);
    virtual QString getTableCreationScript(Connection* c, const QString& tabName);
    virtual QString getViewCreationScript(Connection* c, const QString& name);
    virtual QVector<Sp*> getDatabaseSpecificProperties() const;
    virtual bool supportsStoredMethods() {return false;}
    virtual QString spiExtension(QUuid uid);
    virtual QStringList getKeywords() const;

    QString formatLastError(const QString &header, const QSqlError&);

private:
    SqliteDatabaseEngine();

    static QVector<DatabaseBuiltinFunction> buildFunctions();
    static QVector<Sp*> buildSps();
    static QString provideConnectionName(const QString&);

    void parseTriggerSql(const QString& inSql, QString& outEvent, QString& outTime, QString& sql);

private:

    QMultiMap <UserDataType*, Column*> m_revEngMappings;
    QMap <QString, UserDataType*> m_oneTimeMappings;

private:

    static SqliteDatabaseEngine* s_instance;
    static QVector<DatabaseBuiltinFunction>* s_builtinFunctions;
    static QVector<Sp*>* s_sqliteSpecificProperties;
    static int m_sqliteConnectionCounter;
    static QMutex* m_connectionMutex;

};

#endif // MYSQLDATABASEENGINE_H
