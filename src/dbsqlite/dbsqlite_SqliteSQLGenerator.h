#ifndef SQLITESQLGENERATOR_H
#define SQLITESQLGENERATOR_H

#include <QDebug>

#include "db_BasicSqlGenerator.h"

class DatabaseEngine;
class ForeignKey;

class SqliteSQLGenerator : public BasicSqlGenerator
{
public:

    SqliteSQLGenerator(DatabaseEngine* engine) : BasicSqlGenerator(engine)
    {
    }

    ~SqliteSQLGenerator() {}
    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName, const QMap<QString, QString> &fkMappings, const Connection* pdest) const;

    virtual QString getTableRenameSql(const QString& from, const QString& to);

    virtual QString getAlterTableForNewColumn(const QString& table, const Column* column, const QString& after);


    virtual QString sqlForAColumn(const Column* col) const;

private:

    QString createTableOnlyScript(Table* table, const QStringList &foreignKeys, const QString &tabName, const Connection *pdest) const;
};

#endif // SQLITESQLGENERATOR_H

