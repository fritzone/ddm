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
    virtual QStringList generateAlterTableForForeignKeys(Table* t, const QHash<QString, QString>& options) const;
    virtual QStringList generateCreateStoredMethodSql(StoredMethod* p, const QHash<QString, QString>& options) const;
    virtual QString getTableRenameSql(const QString& from, const QString& to);
    virtual QString getAlterTableForChangeColumnOrder(const QString& table, const Column* column, const QString& afterThis);
    virtual QString getAlterTableForColumnRename(const QString& table, const Column* column, const QString& oldName);
    virtual QString getAlterTableForNewColumn(const QString& table, const Column* column, const QString& after);
    virtual QString getAlterTableForColumnDeletion(const QString& table, const QString& column);
    virtual QString getAlterTableForColumnChange(const QString& table, const Column* col);
    virtual QString getAlterTableToDropForeignKey(const QString& table, const QString& fkName);
    virtual QString getDropTable(const QString& table);
    virtual QStringList getAlterTableForDropForeignKey(const QString& table, const ForeignKey *fk);
    virtual QString getUpdateTableForColumns(const QString& table, const QStringList& pkeys, const QStringList& pvalues, const QString& destCol, const QString& destValue);
    virtual QString getDeleteFromTable(const QString& table, const QStringList& pkeys, const QStringList& pvalues);
    virtual QString getInsertsIntoTable(const QString& table, const QStringList& columns, const QStringList& values);
    virtual QString getDropView(const QString& viewName);
    virtual QString getDropProcedure(const QString& proc);
    virtual QString getDropFunction(const QString& func);
    virtual QString getDropTrigger(const QString& trig);

    virtual QString sqlForAColumn(const Column* col) const;

private:

    QString createTableOnlyScript(Table* table, const QStringList &foreignKeys, const QString &tabName, const Connection *pdest) const;
    QString indexTypeSpecified(Index* idx) const;
    QString getIndexUsedLength(Index* idx, const Column* c) const;
    QString createViewReplaceability(View* v) const;
    QString createViewColumnNames(View *v) const;
};

#endif // SQLITESQLGENERATOR_H

