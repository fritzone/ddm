#ifndef DB_DEFAULTDATABASEENGINE_H
#define DB_DEFAULTDATABASEENGINE_H

#include "db_DatabaseEngine.h"

class ForeignKey;

/**
 * Class doing some work which would be common for most of the database engines
 * and also having methods returning default values for some functions
 */
class DefaultDatabaseEngine : public DatabaseEngine
{
public:

    DefaultDatabaseEngine(const QString& db, const QString& uid) : DatabaseEngine(db, uid)
    {}

    QString toHexString(const QString &x);

    QStringList chopUpString(const QString &x, int size)
    {
        QStringList result;
        QString piece = "";
        int ctr = 0;
        for(int i=0;i <x.length(); i++)
        {
            piece += x.at(i);
            ctr ++;
            if(ctr == size)
            {
                result.push_back(piece);
                piece = "";
                ctr = 0;
            }
        }

        result.push_back(piece);
        return result;
    }

    ForeignKey *createForeignKey(bool& foundAtLeastOneForeignKey,
                                 const QString& referenceeTableName,
                                 Version* v,
                                 const QString& referencedColumnName,
                                 const QString& referenceeColumnName,
                                 const QString& referencedTableName,
                                 const QString &name);

    QString formatLastError(const QString& header, const QSqlError &error);
    QStringList getResultOfQuery(const QString& squery, Connection* conn, const QString &error, int column);
    QStringList getResultOfRepoQuery(Connection *conn, const QString& queryId, const QString& objects);

    virtual QString getDbMetadata(Connection *c);
    virtual bool injectMetadata(Connection *c, const Version *v);
    virtual bool tableBlocksForeignKeyFunctionality(const Table*) const;
    virtual Sp* getSpForSqlRole(const QString& uid) const;
    virtual QString getDefaultDatatypesLocation();
    virtual QStringList getKeywords() const;
    virtual QString getDelimiterKeyword();
    virtual QStringList getTriggerEvents();
    virtual QStringList getTriggerTimings();
    virtual QString spiExtension(QUuid);
    virtual QVector<Sp*> getDatabaseSpecificProperties() const;
    virtual QVector<DatabaseBuiltinFunction> getBuiltinFunctions();
    virtual const DatabaseBuiltinFunction& getBuiltinFunction(const QString& name);
    virtual QStringList getAvailableStoredFunctions(Connection*);
    virtual QStringList getAvailableStoredProcedures(Connection*conn);
    virtual Procedure* reverseEngineerProc(Connection*, const QString&, Version*);
    virtual Function* reverseEngineerFunc(Connection*, const QString&, Version*);
    virtual QStringList getAvailableDatabases(const QString&, const QString&, const QString&, int);
    virtual bool createDatabase(Connection*);
    virtual bool dropDatabase(Connection*);
    virtual bool storedMethodSupport();
    virtual QSet<PROGRAMMING_LANGUAGES> storedMethodLanguages();
    virtual QMap<PARAMETER_FIELD_ROLES, int> parameterFieldOrders();
    virtual QString getDefaultStoredMethodBody(PROGRAMMING_LANGUAGES lang);
    virtual QString getStoredMethodReturnKeyword();
    virtual QString getTriggerBodyDefinitionStatement();
    bool executeSql(Connection* c, const QStringList& sqls, const QStringList &uid, QString& lastSql, bool rollbackOnError);
    virtual QStringList getOnUpdateActions();
    virtual QStringList getOnDeleteActions();
    virtual QStringList getAvailableViews(Connection* c);
    virtual QStringList getAvailableTriggers(Connection* conn);
    virtual QStringList getAvailableTables(Connection* conn);

};

#endif // DB_DEFAULTDATABASEENGINE_H
