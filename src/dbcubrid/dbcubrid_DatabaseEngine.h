#ifndef CUBRIDDATABASEENGINE_H
#define CUBRIDDATABASEENGINE_H

#include "db_DefaultDatabaseEngine.h"

#include <QSqlDatabase>
#include <QMutex>
#include <QStringList>

class UserDataType;
class Column;

class CUBRIDDatabaseEngine : public DefaultDatabaseEngine
{
public:

    static CUBRIDDatabaseEngine* instance();

public:

    virtual ~CUBRIDDatabaseEngine();

    virtual QUuid getClassUid() const;

    virtual void setup();
    virtual bool executeSql(Connection* c, const QStringList& sqls, const QStringList& uid, QString& lastSql, bool rollbackOnError);
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
    virtual bool tryConnect(Connection* c);
    virtual QSqlDatabase getQSqlDatabaseForConnection(Connection *c) ;
    virtual QStringList getColumnsOfTable(Connection* c, const QString& tableName);
    virtual bool dropDatabase(Connection* c);
    virtual Procedure* reverseEngineerProc(Connection *c, const QString& procName, Version *v);
    virtual Function* reverseEngineerFunc(Connection *c, const QString& funcName, Version* v);
    virtual Trigger* reverseEngineerTrigger(Connection *c, const QString& procName, Version* v);
    virtual QString getTableDescriptionScript(const QString& tabName);
    virtual QStringList getAvailableIndexes(Connection* c);
    virtual QString getTableCreationScript(Connection* c, const QString& tabName);
    virtual QString getViewCreationScript(Connection* c, const QString& tabName);
    virtual bool supportsStoredMethods() {return true;}
    virtual QString spiExtension(QUuid) ;

private:
    CUBRIDDatabaseEngine();

private:

    static QVector<DatabaseBuiltinFunction> buildFunctions();
    static QString provideConnectionName(const QString&);

private:

    QMultiMap <UserDataType*, Column*> m_revEngMappings;
    QMap <QString, UserDataType*> m_oneTimeMappings;

private:

    static CUBRIDDatabaseEngine* s_instance;
    static int m_connectionCounter;
    static QMutex* m_connectionMutex;

};

#endif // CUBRIDDATABASEENGINE_H

