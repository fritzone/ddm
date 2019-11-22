#include "dbcubrid_SQLGenerator.h"
#include "dbcubrid_DatabaseEngine.h"

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
#include "ddm_strings.h"
#include "SpInstance.h"
#include "db_DatabaseEngine.h"
#include "Configuration.h"
#include "conn_CUBRID.h"

QStringList CUBRIDSQLGenerator::generateCreateTableSql(Table *table,
                                const QHash<QString, QString> &options,
                                const QString& tabName,
                                const QMap<QString, QString> &fkMappings,
                                const Connection* pdest) const
{
    // error check: Is this a CUBRIDConnection? (if we have one)
    const CUBRIDConnection* dest = dynamic_cast<const CUBRIDConnection*>(pdest);
    if(!dest && pdest)
    {
        return QStringList();
    }

    return BasicSqlGenerator::generateCreateTableSql(table, options, tabName, fkMappings, pdest);
}

QString CUBRIDSQLGenerator::createTableOnlyScript(Table* table, const QStringList& foreignKeys, const QString& tabName, const Connection *) const
{
    bool needfk = true;

    QString createTable = basicCreateTableScript(table, foreignKeys, tabName, needfk);

    // done
    createTable += strSemicolon + strNewline;

    // and here we are done with the create table command
    return createTable;
}

QString CUBRIDSQLGenerator::sqlForAColumn(const Column *col) const
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
                (QString( (dt->getType()==DT_STRING) || (dt->getType()==DT_DATETIME && g.toUpper()!="SYS_TIMESTAMP") ? "\"" : "") + dt->getDefaultValue() +
                QString( (dt->getType()==DT_STRING) || (dt->getType()==DT_DATETIME && g.toUpper()!="SYS_TIMESTAMP") ? "\"" : "") ));
    }

    // and set the auto inc
    if(autoInc == strTrue)
    {
        columnsSql += strSpace + correctCase(m_engine->spiExtension(uidColumnAutoIncrement));
    }

    return columnsSql;
}
