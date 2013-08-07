#include "dbsqlite_SQLGenerator.h"
#include "core_Table.h"
#include "core_UserDataType.h"
#include "core_Column.h"
#include "core_Index.h"
#include "core_ForeignKey.h"
#include "core_TableInstance.h"
#include "core_View.h"
#include "qbr_SelectQuery.h"
#include "core_StoredMethod.h"
#include "core_Trigger.h"
#include "strings.h"
#include "SpInstance.h"
#include "db_DatabaseEngine.h"
#include "dbsqlite_DatabaseEngine.h"
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
    // override the
    QHash<QString, QString> opts = options;
    opts["FKSposition"] = "InTable";
    return BasicSqlGenerator::generateCreateTableSql(table, opts, tabName, fkMappings, pdest);
}

QString SqliteSQLGenerator::sqlForAColumn(const Column *col) const
{
    QString columnsSql = strSpace + col->getName() + strSpace;

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
        columnsSql += correctCase("default") + dt->getDefaultValue();
    }

    // and set the auto inc
    if(autoInc == strTrue)
    {
        columnsSql += strSpace + correctCase(m_engine->spiExtension(uidColumnAutoIncrement));
    }

    return columnsSql;
}

QString SqliteSQLGenerator::getTableRenameSql(const QString& from, const QString& to)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE ") + from + correctCase(" RENAME TO ") + to;
    return res;
}

QString SqliteSQLGenerator::getAlterTableForNewColumn(const QString& table, const Column* column, const QString& /*after*/)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace +
            correctCase("ADD COLUMN") + sqlForAColumn(column);

    return res;
}
