#ifndef MYSQLSQLGENERATOR_H
#define MYSQLSQLGENERATOR_H

#include <QDebug>

#include "db_AdvancedSqlGenerator.h"

class DatabaseEngine;
class ForeignKey;
class Index;
class MySqlConnection;

class MySQLSQLGenerator : public AdvancedSqlGenerator
{
public:

    MySQLSQLGenerator(DatabaseEngine* engine) : AdvancedSqlGenerator(engine)
    {
    }

    ~MySQLSQLGenerator() {}

    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName, const QMap<QString, QString> &fkMappings, const Connection* pdest) const;
    virtual QString getDropProcedure(const QString& proc);
    virtual QString getDropFunction(const QString& func);
    virtual QString sqlForAColumn(const Column* col) const;
    virtual QString backtickedName(const QString& name) const;

private:

    QString createTableOnlyScript(Table* table, const QStringList &foreignKeys, const QString &tabName, const Connection *pdest) const;
    QString indexTypeSpecified(Index* idx) const;

    QString provideCodepageScript(Table* table) const;
    QString provideDatabaseEngineScript(Table* table, const MySqlConnection *dest) const;
    QString provideChecksumScript(Table* table, const MySqlConnection *dest) const;
    QString provideAutoIncrementForTableScript(Table* table) const;
    QString provideAverageRowLengthScript(Table* table) const;
    QString dbEngineName(Table* , const MySqlConnection *dest) const;

};

#endif // MYSQLSQLGENERATOR_H

