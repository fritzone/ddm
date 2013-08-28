#include "db_AdvancedSqlGenerator.h"
#include "core_Table.h"
#include "core_View.h"
#include "core_Column.h"
#include "core_Index.h"
#include "db_SP.h"
#include "SpInstance.h"
#include "db_DatabaseEngine.h"
#include "core_ForeignKey.h"

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


QStringList AdvancedSqlGenerator::generateAlterTableForForeignKeys(Table *t, const QHash<QString, QString> &options) const
{
    QStringList finalSql;
    initForOptions(options);

    for(int j=0; j<t->getForeignKeyCommands().size(); j++)
    {
        QString f = correctCase("alter table");
        f += t->getName();
        f += strSpace + correctCase("add");
        f += t->getForeignKeyCommands().at(j);
        f += strSemicolon + strNewline;

        finalSql << f;
    }
    return finalSql;
}

QString AdvancedSqlGenerator::createViewReplaceability(View* v) const
{
    return spiResult(v, uidViewCanReplace);
}

QString AdvancedSqlGenerator::createViewColumnNames(View *v) const
{
    QString result = "";
    if(v->getColumnNames().size() > 0)
    {
        QString c = strOpenParantheses;
        for(int i=0; i<v->getColumnNames().size(); i++)
        {
            c += v->getColumnNames().at(i);
            if(i<v->getColumnNames().size() - 1)
            {
                c += strComma + strSpace;
            }
        }
        result += c + strCloseParantheses;
    }
    return result;
}

QString AdvancedSqlGenerator::getIndexUsedLength(Index* idx, const Column *c) const
{
    QString result = "";
    // Now see if we have an SPI for the given column
    if(SpInstance* spi = idx->getSpiOfColumnForSpecificRole(c, uidColumnOfIndexLength, m_engine->getDatabaseEngineName()))
    {
        result = strOpenParantheses + spi->get() + strCloseParantheses;
    }
    return result;
}


QString AdvancedSqlGenerator::getRecreateForeignKeySql(ForeignKey* fkI, const QString& foreignKeysTable)
{
    QString foreignKeySql1 = "";
    QString foreignKeySql2 = "";

    for(int j=0; j<fkI->getAssociations().size(); j++)
    {

        ForeignKey::ColumnAssociation* assocJ = fkI->getAssociations().at(j);
        foreignKeySql1 += assocJ->getLocalColumn()->getName();
        foreignKeySql2 += assocJ->getForeignColumn()->getName();

        if(j < fkI->getAssociations().size() - 1)
        {
            foreignKeySql1 += ", ";
            foreignKeySql2 += ", ";
        }
    }
    QString foreignKeySql = " CONSTRAINT " + fkI->getName() + " FOREIGN KEY (";
    foreignKeySql += foreignKeySql1;
    foreignKeySql += ") REFERENCES ";
    foreignKeySql += foreignKeysTable;  // Not an OOP project, this is acceptable FOR NOW
    foreignKeySql += "(" + foreignKeySql2 + ")";
    QString t = fkI->getOnDelete();
    if(t.length() > 0) foreignKeySql += QString(" ") + ("ON DELETE ") + (t);
    t = fkI->getOnUpdate();
    if(t.length() > 0) foreignKeySql += QString(" ") + ("ON UPDATE ") + (t);

    QString f = "ALTER TABLE ";
    f += fkI->getLocalTable()->getName();
    f += " ADD ";
    f += foreignKeySql;

    return f;
}

QString AdvancedSqlGenerator::getDropProcedure(const QString& proc)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP PROCEDURE") + proc;
    return res;
}

QString AdvancedSqlGenerator::getDropFunction(const QString& func)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP FUNCTION") + func;
    return res;
}
