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
#include "strings.h"
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

QString CUBRIDSQLGenerator::getIndexUsedLength(Index* idx, const Column *c) const
{
    QString result = "";
    // Now see if we have an SPI for the given column
    if(SpInstance* spi = idx->getSpiOfColumnForSpecificRole(c, uidMysqlColumnOfIndexLength, m_engine->getDatabaseEngineName()))
    {
        result = strOpenParantheses + spi->get() + strCloseParantheses;
    }
    return result;
}

QStringList CUBRIDSQLGenerator::generateAlterTableForForeignKeys(Table *t, const QHash<QString, QString> &options) const
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

QString CUBRIDSQLGenerator::getDropProcedure(const QString& proc)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP PROCEDURE") + proc;
    return res;
}

QString CUBRIDSQLGenerator::getDropFunction(const QString& func)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP FUNCTION") + func;
    return res;
}

QString CUBRIDSQLGenerator::createViewReplaceability(View* v) const
{
    return spiResult(v, uidMysqlViewCanReplace);
}

QString CUBRIDSQLGenerator::createViewColumnNames(View *v) const
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

QString CUBRIDSQLGenerator::getRecreateForeignKeySql(ForeignKey* fkI, const QString& foreignKeysTable)
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

    // TODO: This is just CUBRID for now
    QString f = "ALTER TABLE ";
    f += fkI->getLocalTable()->getName();
    f += " ADD ";
    f += foreignKeySql;

    return f;
}

QString CUBRIDSQLGenerator::backtickedName(const QString &name) const
{
    QString result = "["+ name + "] ";
    return result;
}
