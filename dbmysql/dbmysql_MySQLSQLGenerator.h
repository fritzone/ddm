#ifndef MYSQLSQLGENERATOR_H
#define MYSQLSQLGENERATOR_H

#include <QDebug>

#include "db_AbstractSQLGenerator.h"

class DatabaseEngine;

class MySQLSQLGenerator : public AbstractSqlGenerator
{
public:

    MySQLSQLGenerator(DatabaseEngine* engine) : m_engine(engine)
    {
    }

    ~MySQLSQLGenerator() {}

    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName, const Connection* dest) const;
    virtual QStringList generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const;
    virtual QStringList generateDefaultValuesSql(Table* table, const QHash<QString, QString>& options) const;
    virtual QStringList generateCreateViewSql(View *v, const QHash<QString, QString> &options) const;
    virtual QStringList generateAlterTableForForeignKeys(Table* t, const QHash<QString, QString>& options) const;
    virtual QStringList generateCreateStoredMethodSql(StoredMethod* p, const QHash<QString, QString>& options) const;
    virtual QStringList generateTriggerSql(Trigger* t, const QHash<QString, QString>& options) const;
    virtual QString getTableRenameSql(const QString& from, const QString& to);
    virtual QString getAlterTableForChangeColumnOrder(const QString& table, const Column* column, const QString& afterThis);
    virtual QString getAlterTableForColumnRename(const QString& table, const Column* column, const QString& oldName);
    virtual QString getAlterTableForNewColumn(const QString& table, const Column* column, const QString& after);
    virtual QString getAlterTableForColumnDeletion(const QString& table, const QString& column);
    virtual QString getAlterTableForColumnChange(const QString& table, const Column* col);
    virtual QString getAlterTableToDropForeignKey(const QString& table, const QString& fkName);
    virtual QString getDropTable(const QString& table);

private:

    QString quotelessString(const QString&) const;
    QString sqlForAColumn(const Column* col, int pkpos, bool backticks, bool upcase) const;

private:

    DatabaseEngine* m_engine;

};

#endif // MYSQLSQLGENERATOR_H
