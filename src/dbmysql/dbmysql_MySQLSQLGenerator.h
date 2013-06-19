#ifndef MYSQLSQLGENERATOR_H
#define MYSQLSQLGENERATOR_H

#include <QDebug>

#include "db_BasicSqlGenerator.h"

class DatabaseEngine;
class ForeignKey;
class Index;
class MySqlConnection;

class MySQLSQLGenerator : public BasicSqlGenerator
{
public:

    MySQLSQLGenerator(DatabaseEngine* engine) : BasicSqlGenerator(engine)
    {
    }

    ~MySQLSQLGenerator() {}

    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName, const QMap<QString, QString> &fkMappings, const Connection* pdest) const;
    virtual QStringList generateAlterTableForForeignKeys(Table* t, const QHash<QString, QString>& options) const;
    virtual QStringList generateCreateStoredMethodSql(StoredMethod* p, const QHash<QString, QString>& options) const;
    virtual QString getTableRenameSql(const QString& from, const QString& to);
    virtual QString getAlterTableForChangeColumnOrder(const QString& table, const Column* column, const QString& afterThis);
    virtual QString getAlterTableForColumnRename(const QString& table, const Column* column, const QString& oldName);
    virtual QString getAlterTableForNewColumn(const QString& table, const Column* column, const QString& after);
    virtual QString getAlterTableForColumnDeletion(const QString& table, const QString& column);
    virtual QString getAlterTableForColumnChange(const QString& table, const Column* col);
    virtual QString getAlterTableToDropForeignKey(const QString& table, const QString& fkName);
    virtual QString getDropProcedure(const QString& proc);
    virtual QString getDropFunction(const QString& func);

    virtual QString sqlForAColumn(const Column* col) const;

private:

    QString createTableOnlyScript(Table* table, const QStringList &foreignKeys, const QString &tabName, const Connection *pdest) const;
    QString indexTypeSpecified(Index* idx) const;
    QString getIndexUsedLength(Index* idx, const Column* c) const;
    QString createViewReplaceability(View* v) const;
    QString createViewColumnNames(View *v) const;

    QString provideCodepage(Table* table) const;
    QString provideDatabaseEngine(Table* table, const MySqlConnection *dest) const;
    QString provideChecksum(Table* table, const MySqlConnection *dest) const;
    QString provideAutoIncrementForTable(Table* table) const;
    QString provideAverageRowLength(Table* table) const;
    QString dbEngineName(Table* , const MySqlConnection *dest) const;

};

#endif // MYSQLSQLGENERATOR_H

