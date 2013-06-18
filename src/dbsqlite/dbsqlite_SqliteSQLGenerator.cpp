#include "dbsqlite_SqliteSQLGenerator.h"
#include "core_Table.h"
#include "core_UserDataType.h"
#include "core_Column.h"
#include "Index.h"
#include "ForeignKey.h"
#include "core_TableInstance.h"
#include "core_View.h"
#include "qbr_SelectQuery.h"
#include "core_StoredMethod.h"
#include "core_Trigger.h"
#include "strings.h"
#include "SpInstance.h"
#include "db_DatabaseEngine.h"
#include "dbsqlite_SqliteDatabaseEngine.h"
#include "Connection.h"
#include "Configuration.h"
#include "SqliteConnection.h"

QStringList SqliteSQLGenerator::generateCreateTableSql(Table *table,
                                    const QHash<QString, QString> &options,
                                    const QString& tabName,
                                    const QMap<QString, QString> &fkMappings,
                                    const Connection* pdest) const
{
    // error check: Is this a MySqlConnection? (if we have one)
    const SqliteConnection* dest = dynamic_cast<const SqliteConnection*>(pdest);
    if(!dest && pdest)
    {
        return QStringList();
    }

    return BasicSqlGenerator::generateCreateTableSql(table, options, tabName, fkMappings, pdest);
}

QString SqliteSQLGenerator::sqlForAColumn(const Column *col) const
{
    QString columnsSql = strSpace + backtickedName(col->getName());

    // column type
    const UserDataType* dt = col->getDataType();
    columnsSql += correctCase(dt->sqlAsString());

    columnsSql += spiResult(col, QUuid(uidMysqlColumnZeroFill));

    // is this primary key?
    if(col->isPk())
    {
        if(m_pkpos == Configuration::PkInColumnDeclaration)
        {
            columnsSql += correctCase("primary key");
        }
    }

    // is this a nullable column?
    if(!dt->isNullable())
    {
        columnsSql += correctCase("not null");
    }

    QString autoInc = strFalse;
    {
    // see if we have "AUTO_INCREMENT" sp. Valid both for Sqlite and Mysql
    SpInstance* spi = col->getInstanceForSqlRoleUid(m_engine, uidColumnAutoIncrement);
    if(spi)
    {
        autoInc = spi->get();
    }
    }

    // do we have a default value for this columns' data type? Only for non auto inc.
    if(dt->getDefaultValue().length() > 0 && autoInc != strTrue)
    {
        QString g = dt->getDefaultValue();
        columnsSql += correctCase("default")+
                (dt->getDefaultValue().toUpper() == "NULL"? ("NULL ") :
                (QString( (dt->getType()==DT_STRING) || (dt->getType()==DT_DATETIME && g.toUpper()!="CURRENT_TIMESTAMP") ? "\"" : "") + dt->getDefaultValue() +
                QString( (dt->getType()==DT_STRING) || (dt->getType()==DT_DATETIME && g.toUpper()!="CURRENT_TIMESTAMP") ? "\"" : "") ));
    }

    // and set the auto inc
    if(autoInc == strTrue)
    {
        columnsSql += strSpace + correctCase(m_engine->spiExtension(uidColumnAutoIncrement));
    }

    // is there a comment for this?
    if(col->getDescription().length())
    {
        columnsSql += QString(m_upcase?" COMMENT ":" comment ") + "\'" + col->getDescription() + "\' ";
    }

    return columnsSql;
}

// sqlite does not really support this
QStringList SqliteSQLGenerator::generateAlterTableForForeignKeys(Table* /*t*/, const QHash<QString, QString>& /*options*/) const
{
    return QStringList();
}

QStringList SqliteSQLGenerator::generateCreateStoredMethodSql(StoredMethod *p, const QHash<QString, QString>& /*options*/) const
{
    QStringList t;
    return t;
}

QString SqliteSQLGenerator::getTableRenameSql(const QString& from, const QString& to)
{
    QString res = "ALTER TABLE " + from + " RENAME TO " + to;
    return res;
}

QString SqliteSQLGenerator::getAlterTableForChangeColumnOrder(const QString& /*table*/, const Column* /*column*/, const QString& /*afterThis*/)
{
    return "-- Sqlite does not really support change of column order";
}

QString SqliteSQLGenerator::getAlterTableForColumnRename(const QString& /*table*/, const Column* /*column*/, const QString& /*oldName*/)
{
    return "-- Sqlite does not really support rename of column";
}

QString SqliteSQLGenerator::getAlterTableForNewColumn(const QString& table, const Column* column, const QString& /*after*/)
{
    QString res = correctCase("ALTER TABLE") + table + strSpace +
            correctCase("ADD COLUMN") + sqlForAColumn(column);

    return res;
}

QString SqliteSQLGenerator::getAlterTableForColumnDeletion(const QString& table, const QString& column)
{
    QString res = "ALTER TABLE " + table + " DROP " + column;
    return res;
}

QString SqliteSQLGenerator::getAlterTableForColumnChange(const QString& /*table*/, const Column* /*col*/)
{
    return "-- Sqlite does not really support change of column";
}

QString SqliteSQLGenerator::getAlterTableToDropForeignKey(const QString& table, const QString& fkName)
{
    QString res = "ALTER TABLE " + table + " DROP FOREIGN KEY " + fkName;
    return res;
}

QString SqliteSQLGenerator::getDropTable(const QString& table)
{
    QString res = "DROP TABLE IF EXISTS " + table;
    return res;
}

QStringList SqliteSQLGenerator::getAlterTableForDropForeignKey(const QString& table, const ForeignKey* fk)
{
    QStringList r;
    QString res = "ALTER TABLE " + table + " DROP FOREIGN KEY " + fk->getName();

    r << res;
    return r;
}

QString SqliteSQLGenerator::getUpdateTableForColumns(const QString& table, const QStringList& pkeys, const QStringList& pvalues, const QString& destCol, const QString& destValue)
{
    QString where = "";
    for(int i=0; i<pkeys.size(); i++)
    {
        where += pkeys[i] + " = \"" + pvalues[i] + "\"";
        if(i < pkeys.size() - 1) where += " AND ";
        else where += " ";
    }
    QString res = "UPDATE " + table + " SET " + destCol + " = \"" + destValue + "\" WHERE " + where;
    return res;
}

QString SqliteSQLGenerator::getDeleteFromTable(const QString& table, const QStringList& pkeys, const QStringList& pvalues)
{
    QString where = "";
    for(int i=0; i<pkeys.size(); i++)
    {
        where += pkeys[i] + " = \"" + pvalues[i] + "\"";
        if(i < pkeys.size() - 1) where += " AND ";
        else where += " ";
    }
    QString res = "DELETE FROM " + table + " WHERE " + where;
    return res;

}

QString SqliteSQLGenerator::getInsertsIntoTable(const QString& table, const QStringList &columns, const QStringList &values)
{
    QString res = "INSERT INTO " + table + " (";
    for(int i=0; i<columns.size(); i++)
    {
        res += columns[i];
        if(i<columns.size() - 1) res += ", ";
    }
    res += ") VALUES (";
    for(int i=0; i<values.size(); i++)
    {
        res += "\"" + values[i] + "\"";
        if(i<values.size() - 1) res += ", ";
    }
    res += ")";

    return res;
}

QString SqliteSQLGenerator::getDropView(const QString& viewName)
{
    QString res = "DROP VIEW IF EXISTS " + viewName;
    return res;
}

QString SqliteSQLGenerator::getDropProcedure(const QString& proc)
{
    QString res = "DROP PROCEDURE IF EXISTS " + proc;
    return res;
}

QString SqliteSQLGenerator::getDropFunction(const QString& func)
{
    QString res = "DROP FUNCTION IF EXISTS " + func;
    return res;
}

QString SqliteSQLGenerator::getDropTrigger(const QString& trig)
{
    QString res = "DROP TRIGGER IF EXISTS " + trig;
    return res;

}


QString SqliteSQLGenerator::createViewReplaceability(View* /*v*/) const
{
    return "";
}

QString SqliteSQLGenerator::createViewColumnNames(View* /*v*/) const
{
    return "";
}



QString SqliteSQLGenerator::createTableOnlyScript(Table* table,
                                            const QStringList& foreignKeys,
                                            const QString& tabName,
                                            const Connection */*pdest*/) const
{
    QString createTable = basicCreateTableScript(table, foreignKeys, tabName, true);
    // done
    createTable += strSemicolon + strNewline;
    return createTable;

}

QString SqliteSQLGenerator::indexTypeSpecified(Index* /*idx*/) const
{
    return "";
}

QString SqliteSQLGenerator::getIndexUsedLength(Index* /*idx*/, const Column* /*c*/) const
{
    return "";
}
