#ifndef CUBRIDSQLGENERATOR_H
#define CUBRIDSQLGENERATOR_H

#include <QDebug>

#include "db_BasicSqlGenerator.h"

class DatabaseEngine;
class ForeignKey;
class Index;
class CUBRIDConnection;

class CUBRIDSQLGenerator : public BasicSqlGenerator
{
public:

    CUBRIDSQLGenerator(DatabaseEngine* engine) : BasicSqlGenerator(engine)
    {
    }

    ~CUBRIDSQLGenerator() {}

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
    virtual QString getRecreateForeignKeySql(ForeignKey* fkI, const QString &foreignKeysTable);


private:

    QString createTableOnlyScript(Table* table, const QStringList &foreignKeys, const QString &tabName, const Connection *pdest) const;
    QString indexTypeSpecified(Index* idx) const;
    QString getIndexUsedLength(Index* idx, const Column* c) const;
    QString createViewReplaceability(View* v) const;
    QString createViewColumnNames(View *v) const;

    QString provideCodepageScript(Table* table) const;
    QString provideAutoIncrementForTableScript(Table* table) const;
    QString provideAverageRowLengthScript(Table* table) const;

};

#endif // CUBRIDSQLGENERATOR_H

