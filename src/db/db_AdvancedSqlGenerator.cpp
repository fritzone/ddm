#include "db_AdvancedSqlGenerator.h"

#include "strings.h"

QString AdvancedSqlGenerator::getTableRenameSql(const QString& from, const QString& to)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());
    QString res = correctCase("rename table") + from + strSpace + correctCase("to") + to;
    return res;
}

QString AdvancedSqlGenerator::getAlterTableForColumnDeletion(const QString& table, const QString& column)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace + correctCase("DROP") + column;
    return res;
}

QString AdvancedSqlGenerator::getAlterTableForNewColumn(const QString& table, const Column* column, const QString& after)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace +
            correctCase("ADD") + sqlForAColumn(column);
    if(after.isEmpty())
    {
        res += strSpace + correctCase("FIRST");
    }
    else
    {
        res += strSpace + correctCase("AFTER") + after;
    }
    return res;
}

QString AdvancedSqlGenerator::getAlterTableForChangeColumnOrder(const QString& table, const Column* column, const QString& afterThis)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace +correctCase("MODIFY COLUMN") + sqlForAColumn(column);
    if(afterThis.isEmpty())
    {
        res += strSpace + correctCase("FIRST");
    }
    else
    {
        res += strSpace + correctCase("AFTER") + afterThis;
    }
    return res;
}


QString AdvancedSqlGenerator::getAlterTableForColumnRename(const QString& table, const Column* column, const QString& oldName)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE")
            + table + strSpace + correctCase("CHANGE")
            + oldName + strSpace + sqlForAColumn(column);

    return res;
}

QString AdvancedSqlGenerator::getAlterTableForColumnChange(const QString& table, const Column* col)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace + correctCase("MODIFY COLUMN") + sqlForAColumn(col);
    return res;
}


QString AdvancedSqlGenerator::getAlterTableToDropForeignKey(const QString& table, const QString& fkName)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace + correctCase("DROP FOREIGN KEY") + fkName;
    return res;
}
