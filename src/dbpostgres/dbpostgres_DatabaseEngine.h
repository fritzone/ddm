#ifndef PostgresDATABASEENGINE_H
#define PostgresDATABASEENGINE_H

#include "db_DefaultDatabaseEngine.h"

#include <QSqlDatabase>
#include <QMutex>
#include <QStringList>

class UserDataType;
class Column;

class PostgresDatabaseEngine : public DefaultDatabaseEngine
{
public:

    static PostgresDatabaseEngine* instance();

public:

    virtual ~PostgresDatabaseEngine();

    virtual QUuid getClassUid() const;
    virtual bool reverseEngineerDatabase(Connection *c, const QStringList& tables, const QStringList& views, const QStringList& procs, const QStringList& funcs, const QStringList& triggers, Project* p, bool relaxed);
    virtual Table* reverseEngineerTable(Connection *c, const QString& tableName, Project* p, bool relaxed, Version* ver);
    virtual View* reverseEngineerView(Connection *c, const QString& viewName, Version* v);
    virtual bool tryConnect(Connection* c);
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c) ;
    virtual QStringList getColumnsOfTable(Connection* c, const QString& tableName);
    virtual Procedure* reverseEngineerProc(Connection *c, const QString& procName, Version *v);
    virtual Function* reverseEngineerFunc(Connection *c, const QString& funcName, Version* v);
    virtual Trigger* reverseEngineerTrigger(Connection *c, const QString& procName, Version* v);
    virtual QString getTableDescriptionScript(const QString& tabName);
    virtual QStringList getAvailableIndexes(Connection* c);
    virtual QString getTableCreationScript(Connection* c, const QString& tabName);
    virtual QString getViewCreationScript(Connection* c, const QString& name);
    QStringList getAvailableDatabases(const QString& host, const QString& user, const QString& pass, int port);


private:
    PostgresDatabaseEngine();

private:

    static QVector<DatabaseBuiltinFunction> buildFunctions();
    static QString provideConnectionName(const QString&);

private:

    QMultiMap <UserDataType*, Column*> m_revEngMappings;
    QMap <QString, UserDataType*> m_oneTimeMappings;

private:

    static PostgresDatabaseEngine* s_instance;
    static int m_connectionCounter;
    static QMutex* m_connectionMutex;

};

#endif // PostgresDATABASEENGINE_H

