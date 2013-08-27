#ifndef DB_ADVANCEDSQLGENERATOR_H
#define DB_ADVANCEDSQLGENERATOR_H

#include "db_BasicSqlGenerator.h"

class DatabaseEngine;

class AdvancedSqlGenerator : public BasicSqlGenerator
{
public:

    AdvancedSqlGenerator(DatabaseEngine* engine) : BasicSqlGenerator(engine) {}

    QString getTableRenameSql(const QString& from, const QString& to);
    QString getAlterTableForColumnDeletion(const QString& table, const QString& column);
    QString getAlterTableForNewColumn(const QString& table, const Column* column, const QString& after);
    QString getAlterTableForChangeColumnOrder(const QString& table, const Column* column, const QString& afterThis);
    QString getAlterTableForColumnRename(const QString& table, const Column* column, const QString& oldName);
    QString getAlterTableForColumnChange(const QString& table, const Column* col);
    QString getAlterTableToDropForeignKey(const QString& table, const QString& fkName);

};

#endif // DB_ADVANCEDSQLGENERATOR_H
