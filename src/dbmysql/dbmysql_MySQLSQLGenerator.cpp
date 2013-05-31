#include "dbmysql_MySQLSQLGenerator.h"
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
#include "dbmysql_MySQLDatabaseEngine.h"
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

    // do not generate any code for a table which has no columns
    if(table->fullColumns().size() == 0)
    {
        return QStringList();
    }

    // initialize basic code generation options
    initForOptions(options);

    // this will be returned
    QStringList toReturn;

    // first comments
    if(m_comments)
    {
        toReturn << generateTableCreationComments(table, tabName);
    }

    // creating the FOREIGN KEY sql(s)...
    QStringList foreignKeys = foreignKeyParticipants(table, fkMappings);

    // then the CREATE TABLE script
    QString createTable = createTableOnlyScript(table, foreignKeys, tabName, dest);
    toReturn << createTable;

    // then creating the comments for the indexes if required
    if(m_comments)
    {
        QString comment = table->fullIndices().size()>0?"\n-- Create the indexes for table " + tabName + "\n":"";
        toReturn << comment;
    }

    // now create the indexes of the table
    appendCreateIndexCommands(table, toReturn, tabName);

    // and check if we have foreign keys
    if(m_fkpos == Configuration::AfterTable && foreignKeys.size() > 0)
    {
        if(m_comments)
        {
            QString comment = "-- Create the foreign keys for table " + tabName + "\n";
            toReturn << comment;
        }

        QString fkCommand = strNewline + correctCase("alter table") + tabName + correctCase("add");
        fkCommand += generateForeignKeys(foreignKeys);
        toReturn << fkCommand;
    }

    return toReturn;
}

void MySQLSQLGenerator::appendCreateIndexCommands(Table* table, QStringList& toReturn, const QString& tabName) const
{
    for(int i=0; i<table->fullIndices().size(); i++)
    {
        Index* idx = table->getIndex(table->fullIndices().at(i));
        if(idx == 0)
        {
            idx = table->getIndexFromParents(table->fullIndices().at(i));
            if(idx == 0)
            {
                throw QString("ERROR: an index was not identified");
            }
        }

        QString indexCommand = correctCase("create");
        indexCommand += spiResult(idx, uidMysqlIndexCategory);
        indexCommand += correctCase("index");
        indexCommand += table->fullIndices().at(i);

        indexCommand += indexTypeSpecified(idx);

        // and the table
        indexCommand += correctCase("on") + tabName + strOpenParantheses;

        for(int j=0; j<idx->getColumns().size(); j++)
        {
            indexCommand += idx->getColumns().at(j)->getName();
            indexCommand += getIndexUsedLength(idx, idx->getColumns().at(j));
            indexCommand += idx->getOrderForColumn(idx->getColumns().at(j));
            if(j<idx->getColumns().size() - 1)
            {
                indexCommand += strComma + strSpace;
            }
        }
        indexCommand += strCloseParantheses + strSemicolon + strNewline;
        toReturn << indexCommand;
    }
}

QString MySQLSQLGenerator::createTableOnlyScript(Table* table, const QStringList& foreignKeys, const QString& tabName, const MySqlConnection *dest) const
{
    QString createTable = startCreateTableCommand(table);

    // now something mysql specific
    createTable += spiResult(table, uidMysqlIfNotExistsTable);

    // table name
    createTable += requestedTableName(tabName);

    // creating the columns
    createTable += generateColumnList(table);

    QStringList primaryKeys = table->primaryKeyColumnsAsStringlist();
    if(primaryKeys.size() > 1) // if there is more than one we force it to be AfterColumns
    {
        m_pkpos = Configuration::PkAfterColumnsDeclaration;
    }

    // are we having primary keys after columns?
    if(m_pkpos == Configuration::PkAfterColumnsDeclaration && !primaryKeys.empty())
    {
        createTable += generatePrimaryKeys(primaryKeys);
    }

    // now check the foreign keys if any
    if(m_fkpos == Configuration::InTable && foreignKeys.size() > 0)
    {
        createTable += strComma + strNewline + generateForeignKeys(foreignKeys);
    }

    // this is closing the create table SQL
    createTable += strNewline + strCloseParantheses + strNewline;

    // extra MySql stuff: DB engien and codepage
    createTable += provideDatabaseEngine(table, dest);
    createTable += provideCodepage(table);

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

QString MySQLSQLGenerator::provideDatabaseEngine(Table *table, const MySqlConnection* dest) const
{
    // see if we have a storage engine
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

QString MySQLSQLGenerator::provideCodepage(Table *table) const
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

QString MySQLSQLGenerator::quotelessString(const QString& in) const
{
    QString result = "";
    for(int i=0; i<in.length(); i++)
    {
        if(in.at(i) != '"') result += in.at(i);
        else result += "\\\"";
    }
    return result;
}

QStringList MySQLSQLGenerator::foreignKeyParticipants(Table *table, const QMap<QString, QString> &fkMappings) const
{
    QStringList foreignKeys;
    for(int i=0; i<table->getForeignKeys().size(); i++)
    {
        // just pre-render the SQL for foreign keys
        QString foreignKeysLocal = "";
        QString foreignKeysOther = "";

        ForeignKey* fkI = table->getForeignKeys().at(i);
        // the list of foreign key SQLs
        QString foreignKeysTable = fkI->getForeignTableName();
        if(fkMappings.contains(fkI->getName()))
        {
            QString fkTabInst = fkMappings[fkI->getName()];
            if(!fkTabInst.isEmpty())
            {
                foreignKeysTable = fkTabInst;
            }
        }
        for(int j=0; j<fkI->getAssociations().size(); j++)
        {

            ForeignKey::ColumnAssociation* assocJ = fkI->getAssociations().at(j);
            foreignKeysLocal += assocJ->getLocalColumn()->getName();
            foreignKeysOther += assocJ->getForeignColumn()->getName();

            if(j < fkI->getAssociations().size() - 1)
            {
                foreignKeysLocal += strComma + strSpace;
                foreignKeysOther += strComma + strSpace;
            }
        }

        QString foreignKeySql = strSpace + correctCase("constraint");
        foreignKeySql += fkI->getName() + strSpace;
        foreignKeySql += correctCase("foreign key") + strOpenParantheses;
        foreignKeySql += foreignKeysLocal + strCloseParantheses;
        foreignKeySql += correctCase("references");
        foreignKeySql += foreignKeysTable;
        foreignKeySql += strOpenParantheses + foreignKeysOther + strCloseParantheses;
        QString t = fkI->getOnDelete();
        if(t.length() > 0)
        {
            foreignKeySql += strSpace + correctCase("on delete") +correctCase(t);
        }
        t = fkI->getOnUpdate();
        if(t.length() > 0)
        {
            foreignKeySql += strSpace + correctCase("on update") + correctCase(t);
        }

        foreignKeys.append(foreignKeySql);

        // and send it back to the table for the final SQL rendering
        table->addForeignKeyCommand(foreignKeySql);
    }
    return foreignKeys;
}

QStringList MySQLSQLGenerator::generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const
{
    initForOptions(options);
    QStringList result;
    int maxValues = 0;

    // find how many values we have, funny loop.
    for(int i=0; i<tableInstance->columns().size(); i++)
    {
        maxValues = tableInstance->values()[tableInstance->columns().at(i)].size();
        break;
    }

    Table* t = tableInstance->table();
    for(int i=0; i<maxValues; i++)
    {
        QString insert = strNewline + correctCase("insert into");
        insert += tableInstance->getName();
        insert += " (";
        for(int j=0; j<tableInstance->columns().size(); j++)
        {
            Column* c = t->getColumn(tableInstance->columns().at(j));
            if(!c) c = t->getColumnFromParents(tableInstance->columns().at(j));
            if(!c)
            {
                return QStringList();
            }

            bool ainc = false;
            {
                SpInstance* spi = c->getInstanceForSqlRoleUid(m_engine, uidColumnAutoIncrement);
                if(spi && spi->get() == strTrue )
                {
                    ainc = true;
                }
            }
            if(!ainc)
            {
                insert += tableInstance->columns().at(j);
                if(j< tableInstance->columns().size() - 1)
                {
                    insert += ", ";
                }
            }

        }

        insert += correctCase(") values (");
        for(int j=0; j<tableInstance->columns().size(); j++)
        {
            QVector<QString> vals = tableInstance->values()[tableInstance->columns().at(j)];
            Column* c = t->getColumn(tableInstance->columns().at(j));
            if(!c) c = t->getColumnFromParents(tableInstance->columns().at(j));
            if(!c)
            {
                return QStringList();
            }
            bool ainc = false;
            {
                SpInstance* spi = c->getInstanceForSqlRoleUid(m_engine, uidColumnAutoIncrement);
                if(spi && spi->get() == strTrue)
                {
                    ainc = true;
                }
            }
            if(!ainc)
            {
                QString val = vals.at(i);
                if(val.toUpper() != "NULL")
                {
                    if(val.length() > 0)
                    {
                        insert += "\"";
                        insert += quotelessString(vals.at(i));
                        insert += "\"";
                    }
                    else
                    {
                        // find the datatype, add the default value here
                        Column* pcol = tableInstance->table()->getColumn(tableInstance->columns().at(j));
                        if(pcol == 0) pcol = tableInstance->table()->getColumnFromParents(tableInstance->columns().at(j));
                        const UserDataType* udt = pcol->getDataType();
                        insert += "\"";
                        insert += udt->getDefaultValue();
                        insert += "\"";
                    }
                }
                else
                {
                    insert += correctCase("null");
                }
                if(j< tableInstance->columns().size() - 1)
                {
                    insert += ", ";
                }
            }
        }
        insert += ");\n";
        result << insert;
    }
    return result;
}

QStringList MySQLSQLGenerator::generateDefaultValuesSql(Table* table, const QHash<QString, QString>& options) const
{
    QStringList result;
    initForOptions(options);
    const QVector <QVector <QString> >& sv = table->getDefaultValues();
    for(int i=0; i<sv.size(); i++)
    {
        const QVector<QString> &rowI = sv[i];
        QString insert = correctCase("insert into");
        insert += table->getName();
        insert += strOpenParantheses;
        for(int j=0; j<rowI.size(); j++)
        {
            insert += table->getColumns().at(j)->getName();
            if(j< rowI.size() - 1)
            {
                insert += strComma + strSpace;
            }
        }

        insert += strCloseParantheses + strSpace + correctCase("values")
                + strOpenParantheses;

        for(int j=0; j<rowI.size(); j++)
        {
            QString val = rowI[j];
            if(val.toUpper() != "NULL")
            {
                if(val.length() > 0)
                {
                    insert += "\"";
                    insert += quotelessString(rowI[j]);
                    insert += "\"";
                }
                else
                {
                    // find the datatype, add the default value here
                    Column* pcol = table->getColumns().at(j);
                    const UserDataType* udt = pcol->getDataType();
                    insert += "\"";
                    insert += udt->getDefaultValue();
                    insert += "\"";
                }
            }
            else
            {
                insert += correctCase("null");
            }
            if(j< rowI.size() - 1)
            {
                insert += strComma + strSpace ;
            }
        }

        insert += strCloseParantheses + strSemicolon + strNewline;
        result << insert;
    }
    return result;
}

QStringList MySQLSQLGenerator::generateCreateViewSql(View *v, const QHash<QString, QString> &options) const
{
    initForOptions(options);
    if(v->isManual())
    {
        QStringList res;
        res.append(v->getManualSql());
        return res;
    }
    else
    {
        QStringList res;

        QString createViewSql = correctCase("create");

        if(v->canReplace())
        {
            createViewSql += correctCase("or replace");
        }
        createViewSql += correctCase("view") + v->getName() + strSpace;

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
            createViewSql += c + strCloseParantheses;
        }

        createViewSql += strSpace + correctCase("as") + strNewline;

        createViewSql += v->getQuery()->get();

        res.append(createViewSql);
        return res;
    }
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

QStringList MySQLSQLGenerator::generateCreateStoredMethodSql(StoredMethod *p, const QHash<QString, QString>& /*options*/) const
{
    QStringList t;
    t.append(p->getSql());
    return t;
}

QStringList MySQLSQLGenerator::generateTriggerSql(Trigger* t, const QHash<QString, QString>& options) const
{
    initForOptions(options);
    QStringList result;
    QString s = correctCase("create trigger");
    s += t->getName() + strSpace;
    s += correctCase(t->getTime()) + strSpace;
    s += correctCase(t->getEvent()) + strSpace;
    s += correctCase("on");
    s += t->getTable() + strSpace;
    s += correctCase("for each row");
    s += strNewline + t->getSql() + strSemicolon + strNewline;
    result << s;
    return result;
}

QString MySQLSQLGenerator::getTableRenameSql(const QString& from, const QString& to)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());
    QString res = correctCase("rename table") + from + strSpace + correctCase("to") + to;
    return res;
}

QString MySQLSQLGenerator::getAlterTableForChangeColumnOrder(const QString& table, const Column* column, const QString& afterThis)
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

QString MySQLSQLGenerator::getAlterTableForColumnRename(const QString& table, const Column* column, const QString& oldName)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE")
            + table + strSpace + correctCase("CHANGE `")
            + oldName + "` "+ sqlForAColumn(column);

    return res;
}

QString MySQLSQLGenerator::getAlterTableForNewColumn(const QString& table, const Column* column, const QString& after)
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

QString MySQLSQLGenerator::getAlterTableForColumnDeletion(const QString& table, const QString& column)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace + correctCase("DROP") + column;
    return res;
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

QString MySQLSQLGenerator::getAlterTableForColumnChange(const QString& table, const Column* col)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace + correctCase("MODIFY COLUMN") + sqlForAColumn(col);
    return res;
}

QString MySQLSQLGenerator::getAlterTableToDropForeignKey(const QString& table, const QString& fkName)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("ALTER TABLE") + table + strSpace + correctCase("DROP FOREIGN KEY") + fkName;
    return res;
}

QString MySQLSQLGenerator::getDropTable(const QString& table)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP TABLE IF EXISTS") + table;
    return res;
}

QStringList MySQLSQLGenerator::getAlterTableForDropForeignKey(const QString& table, const ForeignKey* fk)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QStringList r;
    QString res = correctCase("ALTER TABLE") + table + strSpace + correctCase("DROP FOREIGN KEY") + fk->getName();

    r << res;
    return r;
}

QString MySQLSQLGenerator::getUpdateTableForColumns(const QString& table, const QStringList& pkeys, const QStringList& pvalues, const QString& destCol, const QString& destValue)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString where = "";
    for(int i=0; i<pkeys.size(); i++)
    {
        where += pkeys[i] + " = \"" + pvalues[i] + "\"";
        if(i < pkeys.size() - 1)
        {
            where += correctCase(" AND ");
        }
        else where += " ";
    }
    QString res = correctCase("UPDATE") + table + correctCase(" SET ") + destCol + " = \"" + destValue + correctCase("\" WHERE ") + where;
    return res;
}

QString MySQLSQLGenerator::getDeleteFromTable(const QString& table, const QStringList& pkeys, const QStringList& pvalues)
{
    QString where = "";
    for(int i=0; i<pkeys.size(); i++)
    {
        where += pkeys[i] + " = \"" + pvalues[i] + "\"";
        if(i < pkeys.size() - 1)
        {
            where += correctCase(" AND ");
        }
        else where += " ";
    }
    QString res = correctCase("DELETE FROM") + table + correctCase(" WHERE") + where;
    return res;

}

QString MySQLSQLGenerator::getInsertsIntoTable(const QString& table, const QStringList &columns, const QStringList &values)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("INSERT INTO") + table + " (";
    for(int i=0; i<columns.size(); i++)
    {
        res += columns[i];
        if(i<columns.size() - 1) res += ", ";
    }
    res += correctCase(") VALUES (");
    for(int i=0; i<values.size(); i++)
    {
        res += "\"" + values[i] + "\"";
        if(i<values.size() - 1) res += ", ";
    }
    res += ")";

    return res;
}

QString MySQLSQLGenerator::getDropView(const QString& viewName)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP VIEW IF EXISTS") + viewName;
    return res;
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

QString MySQLSQLGenerator::getDropTrigger(const QString& trig)
{
    initForOptions(Configuration::instance().sqlGenerationOptions());

    QString res = correctCase("DROP TRIGGER IF EXISTS") + trig;
    return res;
}
