#include "dbmysql_SQLGenerator.h"
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
#include "dbmysql_DatabaseEngine.h"
#include "Configuration.h"
#include "MySqlConnection.h"

QStringList MySQLSQLGenerator::generateCreateTableSql(Table *table,
                                const QHash<QString, QString> &options,
                                const QString& tabName,
                                const QMap<QString, QString> &fkMappings,
                                const Connection* pdest) const
{
    // error check: Is this a MySqlConnection? (if we have one)
    const MySqlConnection* dest = dynamic_cast<const MySqlConnection*>(pdest);
    if(!dest && pdest)
    {
        return QStringList();
    }

    return BasicSqlGenerator::generateCreateTableSql(table, options, tabName, fkMappings, pdest);
}

QString MySQLSQLGenerator::createTableOnlyScript(Table* table, const QStringList& foreignKeys, const QString& tabName, const Connection *pdest) const
{
    const MySqlConnection* dest = dynamic_cast<const MySqlConnection*>(pdest);

    bool needfk = true;

    if(dbEngineName(table, dest).toUpper() != "INNODB")
    {
        needfk = false;
    }

    QString createTable = basicCreateTableScript(table, foreignKeys, tabName, needfk);

    // extra MySql stuff: DB engine and codepage
    createTable += provideDatabaseEngineScript(table, dest);
    createTable += provideCodepageScript(table);

    // more extra mysql stuff: checksum, autoIncrement, etc...
    createTable += provideAutoIncrementForTableScript(table);
    createTable += provideAverageRowLengthScript(table);
    createTable += provideChecksumScript(table, dest);

    // done
    createTable += strSemicolon + strNewline;

    // and here we are done with the create table command
    return createTable;
}

QString MySQLSQLGenerator::getIndexUsedLength(Index* idx, const Column *c) const
{
    QString result = "";
    // Now see if we have an SPI for the given column
    if(SpInstance* spi = idx->getSpiOfColumnForSpecificRole(c, uidMysqlColumnOfIndexLength, m_engine->getDatabaseEngineName()))
    {
        result = strOpenParantheses + spi->get() + strCloseParantheses;
    }
    return result;
}

QString MySQLSQLGenerator::indexTypeSpecified(Index *idx) const
{
    QString result = " ";
    // See if this index has BTREE or HASH specified
    {
    SpInstance* spi = idx->getInstanceForSqlRoleUid(m_engine, uidMysqlIndexType);
    if(spi)
    {
        QString t = spi->get();
        if(t.length())
        {
            result = strSpace + correctCase("USING") + correctCase(t) + strSpace;
        }
    }
    }

    return result;
}

QStringList MySQLSQLGenerator::generateAlterTableForForeignKeys(Table *t, const QHash<QString, QString> &options) const
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

QString MySQLSQLGenerator::backtickedName(const QString& name) const
{
    QString result = (m_backticks?"`":"") + name;
    result += m_backticks?"`":"";
    result += " ";
    return result;
}

QString MySQLSQLGenerator::sqlForAColumn(const Column *col) const
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

QString MySQLSQLGenerator::getDropProcedure(const QString& proc)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP PROCEDURE IF EXISTS") + proc;
    return res;
}

QString MySQLSQLGenerator::getDropFunction(const QString& func)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP FUNCTION IF EXISTS") + func;
    return res;
}

QString MySQLSQLGenerator::createViewReplaceability(View* v) const
{
    return spiResult(v, uidMysqlViewCanReplace);
}

QString MySQLSQLGenerator::createViewColumnNames(View *v) const
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

QString MySQLSQLGenerator::dbEngineName(Table* table, const MySqlConnection* dest) const
{
    // see if we have a storage eng ine
    QString result = "";
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlStorageEngineTable);
    if(spi)
    {
        QString storageEngine = spi->get();
        if(storageEngine.length())
        {
            MySQLDatabaseEngine* mysEng = dynamic_cast<MySQLDatabaseEngine*>(m_engine);
            if(mysEng)
            {
                if(dest)
                {
                    QStringList supportedStroageEngines = mysEng->getSupportedStorageEngines(dest->getHost(), dest->getUser(), dest->getPassword(), dest->getPort());
                    if(supportedStroageEngines.contains(storageEngine.toUpper()))
                    {
                        return storageEngine;
                    }
                }
                else
                {
                    return storageEngine;
                }
            }
        }
    }
    return "";

}

QString MySQLSQLGenerator::provideDatabaseEngineScript(Table *table, const MySqlConnection* dest) const
{
    // see if we have a storage eng ine
    QString result = "";
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlStorageEngineTable);
    if(spi)
    {
        QString storageEngine = spi->get();
        if(storageEngine.length())
        {
            MySQLDatabaseEngine* mysEng = dynamic_cast<MySQLDatabaseEngine*>(m_engine);
            if(mysEng)
            {
                if(dest)
                {
                    QStringList supportedStroageEngines = mysEng->getSupportedStorageEngines(dest->getHost(), dest->getUser(), dest->getPassword(), dest->getPort());
                    if(supportedStroageEngines.contains(storageEngine.toUpper()))
                    {
                        result = correctCase("engine = ")+ storageEngine;
                    }
                }
                else
                {
                    result = correctCase("engine = ")+ storageEngine;
                }
            }
        }
    }
    return result;
}

QString MySQLSQLGenerator::provideChecksumScript(Table *table, const MySqlConnection *dest) const
{
    if(dbEngineName(table, dest).toUpper() != "MYISAM")
    {
        return "";
    }
    QString result("");
    // and the codepage
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlChecksumTable);
    if(spi)
    {
        QString checksum = spi->get();
        if(checksum.length() && checksum != spi->getClass()->getDefaultValue())
        {
            QString aincCmd = correctCase("checksum");
            QString value = checksum;

            result = strSpace + aincCmd + strSpace + value + strSpace;
        }
    }
    return result;

}

QString MySQLSQLGenerator::provideAutoIncrementForTableScript(Table *table) const
{
    QString result("");
    // and the codepage
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlAutoincrementTable);
    if(spi)
    {
        QString autoInc = spi->get();
        if(autoInc.length() && autoInc != spi->getClass()->getDefaultValue())
        {
            QString aincCmd = correctCase("auto_increment");
            QString value = autoInc;

            result = strSpace + aincCmd + strSpace + value + strSpace;
        }
    }
    return result;
}

QString MySQLSQLGenerator::provideAverageRowLengthScript(Table *table) const
{
    QString result("");
    // and the codepage
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlAvgRowLengthTable);
    if(spi)
    {
        QString autoInc = spi->get();
        if(autoInc.length() && autoInc != spi->getClass()->getDefaultValue())
        {
            QString aincCmd = correctCase("avg_row_length");
            QString value = autoInc;

            result = strSpace + aincCmd + strSpace + value + strSpace;
        }
    }
    return result;
}

QString MySQLSQLGenerator::provideCodepageScript(Table *table) const
{
    QString result("");
    // and the codepage
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlCodepageTable);
    if(spi)
    {
        QString codepage = spi->get();
        if(codepage.length())
        {
            QString charset = codepage.left(codepage.indexOf('_'));
            QString dcs = correctCase("DEFAULT CHARACTER SET");
            QString collate = correctCase("COLLATE");

            result = strSpace + dcs + charset + strSpace + collate + codepage;
        }
    }
    return result;
}

QString MySQLSQLGenerator::getRecreateForeignKeySql(ForeignKey* fkI, const QString& foreignKeysTable)
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

    // TODO: This is just MySQL for now
    QString f = "ALTER TABLE ";
    f += fkI->getLocalTable()->getName();
    f += " ADD ";
    f += foreignKeySql;

    return f;
}
