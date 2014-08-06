#ifndef PostgresSQLGENERATOR_H
#define PostgresSQLGENERATOR_H

#include <QDebug>

#include "db_AdvancedSqlGenerator.h"

class DatabaseEngine;
class ForeignKey;
class Index;
class PostgresConnection;

class PostgresSQLGenerator : public AdvancedSqlGenerator
{
public:

    PostgresSQLGenerator(DatabaseEngine* engine) : AdvancedSqlGenerator(engine){}

    ~PostgresSQLGenerator() {}

    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName, const QMap<QString, QString> &fkMappings, const Connection* pdest) const;
    virtual QString sqlForAColumn(const Column* col) const;

private:

    QString createTableOnlyScript(Table* table, const QStringList &foreignKeys, const QString &tabName, const Connection *pdest) const;
};

#endif // PostgresSQLGENERATOR_H
