#ifndef MYSQLDATABASEENGINE_H
#define MYSQLDATABASEENGINE_H

#include "DatabaseEngine.h"

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

    virtual bool injectSql(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QStringList& sqls, QString& lastSql);

    virtual QString getDefaultDatatypesLocation();

};

#endif // MYSQLDATABASEENGINE_H
