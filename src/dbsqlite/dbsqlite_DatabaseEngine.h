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
    virtual bool reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, const QStringList& procs, const QStringList& funcs, const QStringList& triggers, Project* p, bool relaxed);
    virtual Table* reverseEngineerTable(Connection *c, const QString& tableName, Project* p, bool relaxed, Version* ver);
    virtual View* reverseEngineerView(Connection *c, const QString& viewName, Version* v);
    virtual bool tryConnect(Connection* c);
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c);
    virtual QStringList getColumnsOfTable(Connection* c, const QString& tableName);
    virtual Trigger* reverseEngineerTrigger(Connection *c, const QString& procName, Version* v);
    virtual QString getTableDescriptionScript(const QString& tabName);
    virtual QStringList getAvailableIndexes(Connection* c);
    virtual QString getTableCreationScript(Connection* c, const QString& tabName);
    virtual QString getViewCreationScript(Connection* c, const QString& name);

private:
    SqliteDatabaseEngine();

    static QString provideConnectionName(const QString&);

    void parseTriggerSql(const QString& inSql, QString& outEvent, QString& outTime, QString& sql);

private:

    QMultiMap <UserDataType*, Column*> m_revEngMappings;
    QMap <QString, UserDataType*> m_oneTimeMappings;

private:

    static SqliteDatabaseEngine* s_instance;
    static int m_sqliteConnectionCounter;
    static QMutex* m_connectionMutex;

};

#endif // MYSQLDATABASEENGINE_H
