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
#include "Connection.h"
#include "Configuration.h"

QStringList MySQLSQLGenerator::generateCreateTableSql(Table *table, const QHash<QString, QString> &options, const QString& tabName, const QMap<QString, QString> &fkMappings, const Connection* dest) const
{
    // do not generate any code for a table which has no columns
    if(table->fullColumns().size() == 0) return QStringList();

    bool upcase = options.contains("Case") && options["Case"] == "Upper";
    bool comments = false; //options.contains("GenerateComments") && options["GenerateComments"] == "Yes";
    bool backticks = options.contains("Backticks") && options["Backticks"] == "Yes";
    // primary key pos = 0 if the primary key is specified in the column declaration
    // it is 1 if the primary key is defined in a PRIMARY KEY section in the table definition
    // it is 2 if the primary key is defined in an alter table section after the table definition
    int pkpos = 1;
    if(options.contains("PKSposition"))
    {
        if(options["PKSposition"]=="ColumnDeclaration")
        {
            pkpos = 0;
        }
        if(options["PKSposition"]=="AfterColumnsDeclaration")
        {
            pkpos = 1;
        }
        if(options["PKSposition"]=="AfterTableDeclaration")
        {
            pkpos = 2;
        }
    }
    // foreign key pos: 1 in the table script, 2 in an alter script after the table, 3 - not now, just update the table
    Configuration::ForeignKeyPosition fkpos = Configuration::InTable;
    if(options.contains("FKSposition"))
    {
        if(options["FKSposition"] == "InTable")
        {
            fkpos = Configuration::InTable;
        }
        if(options["FKSposition"] == "AfterTable")
        {
            fkpos = Configuration::AfterTable;
        }
        if(options["FKSposition"] == "OnlyInternal")
        {
            fkpos = Configuration::OnlyInternal;
        }
    }

    // the list of primary key columns, used only if pkpos is 1 or 2
    QStringList primaryKeys;

    // the list of foreign key SQLs
    QStringList foreignKeys;
    QString foreignKeysTable = "";

    QStringList toReturn;

    if(comments)
    {
        QString comment;
        QString desc = table->getDescription();
        QStringList lines = desc.split("\n");
        if(lines.size())
        {
            for(int i=0; i< lines.size(); i++)
            {
                comment += QString("-- ") +  lines.at(i) + strNewline;
            }
        }
        else
        {
            comment = QString("-- ") +  (desc.length()>0?desc:" Create table " + tabName) + "\n";
        }
        toReturn << comment;
    }
    QString createTable = upcase? "CREATE " : "create ";

    {
    // see if this is a temporary table
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlTemporaryTable);
    if(spi)
    {
        QString temporary = spi->get();
        if(temporary == "TRUE")
        {
            createTable += upcase? "TEMPORARY ":"temporary ";
        }
    }
    }

    createTable += !upcase? "table ":"TABLE ";

    {
    // see if we have "IF NOT EXISTS" checked
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlIfNotExistsTable);
    if(spi)
    {
        QString ifNotExists = spi->get();
        if(ifNotExists == "TRUE")
        {
            createTable += upcase? "IF NOT EXISTS ":"if not exists ";
        }
    }
    }

    // table name
    createTable += backticks?"`":"";
    createTable += tabName;
    createTable += backticks?"`":"";

    for(int i=0; i<table->fullColumns().size(); i++)
    {
        Column *col = table->getColumn(table->fullColumns()[i]);
        if(col == 0)
        {
            col = table->getColumnFromParents(table->fullColumns()[i]);
            if(col == 0)
            {
                return QStringList("ERROR");
            }
        }

        if(col->isPk())
        {
            primaryKeys.append(table->fullColumns()[i]);
        }

    }

    int x = primaryKeys.size();
    if(x > 1)
    {
        pkpos = 1;
    }

    createTable += "\n(\n";

    // creating the columns
    for(int i=0; i<table->fullColumns().size(); i++)
    {
        // column name


        Column *col = table->getColumn(table->fullColumns()[i]);
        if(col == 0)
        {
            col = table->getColumnFromParents(table->fullColumns()[i]);
            if(col == 0)
            {
                return QStringList("ERROR");
            }
        }

        createTable += sqlForAColumn(col, pkpos, backticks, upcase);
        // do we have more columns after this?
        if(i<table->fullColumns().size() - 1)
        {
            createTable += ",\n";
        }
    }

    // are we having primary keys after columns?
    if(pkpos == 1 && primaryKeys.size() > 0)
    {
        createTable += "\n\t,";
        createTable += upcase?"PRIMARY KEY ":"primary key ";
        createTable += "(";
        for(int i=0; i<primaryKeys.size(); i++)
        {
            createTable += primaryKeys[i];
            if(i<primaryKeys.size() - 1)
            {
                createTable += ", ";
            }
        }
        createTable += ")";
    }


    // creating the FOREIGN KEY sql(s)...
    for(int i=0; i<table->getForeignKeys().size(); i++)
    {
        // just pre-render the SQL for foreign keys
        QString foreignKeySql1 = "";
        QString foreignKeySql2 = "";

        ForeignKey* fkI = table->getForeignKeys().at(i);
        foreignKeysTable = fkI->getForeignTableName();
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
            foreignKeySql1 += assocJ->getLocalColumn()->getName();
            foreignKeySql2 += assocJ->getForeignColumn()->getName();

            if(j < fkI->getAssociations().size() - 1)
            {
                foreignKeySql1 += ", ";
                foreignKeySql2 += ", ";
            }
        }
        QString foreignKeySql = upcase?" CONSTRAINT " : " constraint ";
        foreignKeySql += fkI->getName();
        foreignKeySql += upcase?" FOREIGN KEY (":" foreign key(";
        foreignKeySql += foreignKeySql1;
        foreignKeySql += upcase?") REFERENCES ":") references ";
        foreignKeySql += foreignKeysTable;
        foreignKeySql += "(" + foreignKeySql2 + ")";
        QString t = fkI->getOnDelete();
        if(t.length() > 0) foreignKeySql += QString(" ") + (!upcase?"on delete ":"ON DELETE ") + (upcase?t.toUpper():t.toLower());
        t = fkI->getOnUpdate();
        if(t.length() > 0) foreignKeySql += QString(" ") + (!upcase?"on update ":"ON UPDATE ") + (upcase?t.toUpper():t.toLower());

        foreignKeys.append(foreignKeySql);

        // and send it back to the table for the final SQL rendering
        table->addForeignKeyCommand(foreignKeySql);
    }

    // now check the foreign keys if any
    if(fkpos == Configuration::InTable && foreignKeys.size() > 0)
    {
        createTable += ",\n";
        for(int i=0; i<foreignKeys.size(); i++)
        {
            createTable += "\t" + foreignKeys[i];
            if(i<foreignKeys.size() - 1)
            {
                createTable += ",\n";
            }
        }
    }

    // this is closing the create table SQL
    createTable += "\n)\n";

    {
    // see if we have a storage engine
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
                        createTable += QString(upcase?"ENGINE = ":"engine = ") + storageEngine;
                    }
                }
                else
                {
                    createTable += QString(upcase?"ENGINE = ":"engine = ") + storageEngine;
                }
            }
        }
    }
    }

    {
    // and the codepage
    SpInstance* spi = table->getInstanceForSqlRoleUid(m_engine, uidMysqlCodepageTable);
    if(spi)
    {
        QString codepage = spi->get();
        if(codepage.length())
        {
            QString charset = codepage.left(codepage.indexOf('_'));
            createTable += upcase?(" DEFAULT CHARACTER SET " + charset + " COLLATE " + codepage):(" default character set "  + charset + " collate " + codepage);
        }
    }
    }

    createTable += ";\n\n";
    // and here we are done with the create table command
    toReturn << createTable;

    if(comments)
    {
        QString comment = table->fullIndices().size()>0?"-- Create the indexes for table " + tabName + "\n":"";
        toReturn << comment;
    }

    // now create the indexes of the table
    for(int i=0; i<table->fullIndices().size(); i++)
    {
        QString indexCommand = upcase?"CREATE":"create";
        Index* idx = table->getIndex(table->fullIndices().at(i));

        {
        // and the index category
        SpInstance* spi = idx->getInstanceForSqlRoleUid(m_engine, uidMysqlIndexCategory);
        if(spi)
        {
            QString c = spi->get();
            if(c.length())
            {
                indexCommand += strSpace + c;
            }
        }
        }

        indexCommand += strSpace + (upcase?"INDEX ":"index ");
        indexCommand += table->fullIndices().at(i);

        if(idx == 0)
        {
            idx = table->getIndexFromParents(table->fullIndices().at(i));
            if(idx == 0)
            {
                return QStringList("ERROR");
            }
        }
        // See if this index has BTREE or HASH specified
        {
        SpInstance* spi = idx->getInstanceForSqlRoleUid(m_engine, uidMysqlIndexType);
        if(spi)
        {
            QString t = spi->get();
            if(t.length())
            {
                indexCommand += upcase?(" USING " + t.toUpper() + " ")
                                     :(" using " + t.toLower() + " ");
            }
        }
        }
        // and the table
        indexCommand +=upcase?" ON ":" on ";
        indexCommand += tabName;
        indexCommand += "(";

        for(int j=0; j<idx->getColumns().size(); j++)
        {
            indexCommand += idx->getColumns().at(j)->getName();

            // Now see if we have an SPI for the given column
            if(SpInstance* spi = idx->getSpiOfColumnForSpecificRole(idx->getColumns().at(j), uidMysqlColumnOfIndexLength, m_engine->getDatabaseEngineName()))
            {
                indexCommand += "(" + spi->get() + ")";
            }
            indexCommand += " "+idx->getOrderForColumn(idx->getColumns().at(j));
            if(j<idx->getColumns().size() - 1)
            {
                indexCommand += ", ";
            }
        }
        indexCommand += ")";
        indexCommand +=";";
        indexCommand +="\n\n";

        toReturn << indexCommand;
    }

    // and check if we have foreign keys
    if(fkpos == Configuration::AfterTable && foreignKeys.size() > 0)
    {
        if(comments)
        {
            QString comment = "-- Create the foreign keys for table " + tabName + "\n";
            toReturn << comment;
        }

        QString fkCommand = upcase?"\nALTER TABLE" + tabName :"\nalter table " + tabName;
        fkCommand += upcase?" ADD":" add";
        for(int i=0; i<foreignKeys.size(); i++)
        {
            fkCommand += foreignKeys[i];
            if(i<foreignKeys.size() - 1)
            {
                fkCommand += ",\n";
            }
        }

        toReturn << fkCommand;
    }

    return toReturn;
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

QStringList MySQLSQLGenerator::generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const
{
    QStringList result;
    int maxValues = 0;
    bool upcase = options.contains("Case") && options["Case"] == "Upper";
    Table* t = tableInstance->table();

    // find how many values we have
    for(int i=0; i<tableInstance->columns().size(); i++)
    {
        maxValues = tableInstance->values()[tableInstance->columns().at(i)].size();
        break;
    }
    for(int i=0; i<maxValues; i++)
    {
        QString insert = upcase?"\nINSERT INTO ":"insert into ";
        insert += tableInstance->getName();
        insert += " (";
        for(int j=0; j<tableInstance->columns().size(); j++)
        {
            Column* c = t->getColumn(tableInstance->columns().at(j));
            if(!c) c = t->getColumnFromParents(tableInstance->columns().at(j));
            if(!c)
            {
//                qDebug() << "cannot find c: " << c->getName();
                return QStringList();
            }
            bool ainc = false;
            {
                SpInstance* spi = c->getInstanceForSqlRoleUid(m_engine, uidMysqlColumnAutoIncrement);
                if(spi)
                {
                    QString autoInc = spi->get();
                    if(autoInc == "TRUE")
                    {
                        ainc = true;
                    }
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
        insert += upcase?" ) VALUES (" : ") values (";
        for(int j=0; j<tableInstance->columns().size(); j++)
        {
            QVector<QString> vals = tableInstance->values()[tableInstance->columns().at(j)];
            Column* c = t->getColumn(tableInstance->columns().at(j));
            if(!c) c = t->getColumnFromParents(tableInstance->columns().at(j));
            if(!c)
            {
//                qDebug() << "cannot find c: " << c->getName();
                return QStringList();
            }
            bool ainc = false;
            {
                SpInstance* spi = c->getInstanceForSqlRoleUid(m_engine, uidMysqlColumnAutoIncrement);
                if(spi)
                {
                    QString autoInc = spi->get();
                    if(autoInc == "TRUE")
                    {
                        ainc = true;
                    }
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
                    insert += upcase?"NULL":"null";
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
    const QVector <QVector <QString> >& sv = table->getDefaultValues();
    bool upcase = options.contains("Case") && options["Case"] == "Upper";
    for(int i=0; i<sv.size(); i++)
    {
        const QVector<QString> &rowI = sv[i];
        QString insert = upcase?"INSERT INTO ":"insert into";
        insert += table->getName();
        insert += "(";
        for(int j=0; j<rowI.size(); j++)
        {
            insert += table->getColumns().at(j)->getName();
            if(j< rowI.size() - 1)
            {
                insert += ", ";
            }
        }
        insert += upcase?" ) VALUES (" : ") values (";

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
                insert += upcase?"NULL":"null";
            }
            if(j< rowI.size() - 1)
            {
                insert += ", ";
            }
        }

        insert += ");\n\n";
        result << insert;

    }
    return result;
}

QStringList MySQLSQLGenerator::generateCreateViewSql(View *v, const QHash<QString, QString> &/*options*/) const
{
    if(v->isManual())
    {
        QStringList res;
        res.append(v->getManualSql());
        return res;
    }
    else
    {
        QStringList res;
        bool upcase = true; /*options.contains("Case") && options["Case"] == "Upper";*/
        res.append(upcase?"CREATE":"create");
        if(v->canReplace())
        {
            res.append(upcase?"OR REPLACE":"or replace");
        }
        res.append(QString(upcase?"VIEW ":"view ") + v->getName());
        if(v->getColumnNames().size() > 0)
        {
            QString c = "(";
            for(int i=0; i<v->getColumnNames().size(); i++)
            {
                c += v->getColumnNames().at(i);
                if(i<v->getColumnNames().size() - 1) c += ", ";
            }
            res.append(c + ")");
        }
        res.append(upcase?"AS":"as");
        res.append(strNewline);
        res.append(v->getQuery()->get());
        QString g;
        for(int i=0; i< res.size(); i++)
        {
            g += res.at(i);
            if(res.at(i) != strNewline)
            {
                g += " ";
            }
        }
        res.clear();
        res.append(g);
        return res;
    }
}

QStringList MySQLSQLGenerator::generateAlterTableForForeignKeys(Table *t, const QHash<QString, QString> &options) const
{
    QStringList finalSql;
    bool upcase = options.contains("Case") && options["Case"] == "Upper";
    for(int j=0; j<t->getForeignKeyCommands().size(); j++)
    {
        QString f = upcase?"ALTER TABLE ":"alter table ";
        f += t->getName();
        f += upcase?" ADD ":" add ";
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
    QStringList result;
    bool upcase = options.contains("Case") && options["Case"] == "Upper";
    QString s = upcase?  "CREATE TRIGGER ": "create trigger ";
    s += t->getName(); s+= strSpace;
    s += t->getTime(); s+= strSpace;
    s += t->getEvent(); s+= strSpace;
    s += upcase? "ON ":"on ";
    s += t->getTable(); s+= strSpace;
    s += upcase? "FOR EACH ROW ":"for each row ";
    s += strNewline + t->getSql() + strSemicolon + strNewline;
    result << s;
    return result;
}

QString MySQLSQLGenerator::getTableRenameSql(const QString& from, const QString& to)
{
    QString res = "RENAME TABLE " + from + " TO " + to;
    return res;
}

QString MySQLSQLGenerator::getAlterTableForChangeColumnOrder(const QString& table, const Column* column, const QString& afterThis)
{
    QString res = "ALTER TABLE " + table + " MODIFY COLUMN " + sqlForAColumn(column, -1, true, true);
    if(afterThis.isEmpty())
    {
        res += " FIRST";
    }
    else
    {
        res += " AFTER " + afterThis;
    }
    return res;
}

QString MySQLSQLGenerator::getAlterTableForColumnRename(const QString& table, const Column* column, const QString& oldName)
{
    QString res = "ALTER TABLE " + table + " CHANGE `" + oldName + "` "+ sqlForAColumn(column, -1, true, true);
    return res;
}

QString MySQLSQLGenerator::getAlterTableForNewColumn(const QString& table, const Column* column, const QString& after)
{
    QString res = "ALTER TABLE " + table + " ADD " + sqlForAColumn(column, -1, true, true);
    if(after.isEmpty())
    {
        res += " FIRST";
    }
    else
    {
        res += " AFTER " + after;
    }
    return res;
}

QString MySQLSQLGenerator::getAlterTableForColumnDeletion(const QString& table, const QString& column)
{
    QString res = "ALTER TABLE " + table + " DROP " + column;
    return res;
}

QString MySQLSQLGenerator::sqlForAColumn(const Column *col, int pkpos, bool backticks, bool upcase) const
{
    QString createTable = " ";
    createTable += (backticks?"`":"") + col->getName();
    createTable += backticks?"`":"";
    createTable += " ";

    // column type
    const UserDataType* dt = col->getDataType();
    createTable += upcase?dt->sqlAsString().toUpper():dt->sqlAsString().toLower();

    {
    // zero fill SP?
    SpInstance* spi = col->getInstanceForSqlRoleUid(m_engine, uidMysqlColumnZeroFill);
    if(spi)
    {
        QString zeroFill = spi->get();
        if(zeroFill == "TRUE")
        {
            createTable += upcase? " ZEROFILL ":" zerofill ";
        }
    }
    }

    // is this primary key?
    if(col->isPk())
    {
        if(pkpos == 0)
        {
            createTable += upcase?" PRIMARY KEY ":" primary key ";
        }
    }

    // is this a nullable column?
    if(!dt->isNullable())
    {
        createTable += upcase? " NOT NULL " : " not null ";
    }

    QString autoInc = "FALSE";
    {
    // see if we have "AUTO_INCREMENT" sp
    SpInstance* spi = col->getInstanceForSqlRoleUid(m_engine, uidMysqlColumnAutoIncrement);
    if(spi)
    {
        autoInc = spi->get();
    }
    }

    // do we have a default value for this columns' data type? Only for non auto inc.
    if(dt->getDefaultValue().length() > 0 && autoInc != "TRUE")
    {
        QString g = dt->getDefaultValue();
        createTable += QString(upcase?" DEFAULT ":" default ") +
                (dt->getDefaultValue().toUpper() == "NULL"? ("NULL ") :
                (QString( (dt->getType()==DT_STRING) || (dt->getType()==DT_DATETIME && g.toUpper()!="CURRENT_TIMESTAMP") ? "\"" : "") + dt->getDefaultValue() +
                QString( (dt->getType()==DT_STRING) || (dt->getType()==DT_DATETIME && g.toUpper()!="CURRENT_TIMESTAMP") ? "\"" : "") ));
    }

    // and set the auto inc
    if(autoInc == "TRUE")
    {
        createTable += upcase? " AUTO_INCREMENT ":" auto_increment ";
    }

    // is there a comment for this?
    if(col->getDescription().length())
    {
        createTable += QString(upcase?" COMMENT ":" comment ") + "\'" + col->getDescription() + "\' ";
    }

    return createTable;
}

QString MySQLSQLGenerator::getAlterTableForColumnChange(const QString& table, const Column* col)
{
    QString res = "ALTER TABLE " + table + " MODIFY COLUMN " + sqlForAColumn(col, -1, true, true);
    return res;
}

QString MySQLSQLGenerator::getAlterTableToDropForeignKey(const QString& table, const QString& fkName)
{
    QString res = "ALTER TABLE " + table + " DROP FOREIGN KEY " + fkName;
    return res;
}

QString MySQLSQLGenerator::getDropTable(const QString& table)
{
    QString res = "DROP TABLE IF EXISTS " + table;
    return res;
}

QStringList MySQLSQLGenerator::getAlterTableForDropForeignKey(const QString& table, const ForeignKey* fk)
{
    QStringList r;
    QString res = "ALTER TABLE " + table + " DROP FOREIGN KEY " + fk->getName();

    r << res;
    return r;
}

QString MySQLSQLGenerator::getUpdateTableForColumns(const QString& table, const QStringList& pkeys, const QStringList& pvalues, const QString& destCol, const QString& destValue)
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

QString MySQLSQLGenerator::getDeleteFromTable(const QString& table, const QStringList& pkeys, const QStringList& pvalues)
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

QString MySQLSQLGenerator::getInsertsIntoTable(const QString& table, const QStringList &columns, const QStringList &values)
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

QString MySQLSQLGenerator::getDropView(const QString& viewName)
{
    QString res = "DROP VIEW IF EXISTS " + viewName;
    return res;
}

QString MySQLSQLGenerator::getDropProcedure(const QString& proc)
{
    QString res = "DROP PROCEDURE IF EXISTS " + proc;
    return res;
}

QString MySQLSQLGenerator::getDropFunction(const QString& func)
{
    QString res = "DROP FUNCTION IF EXISTS " + func;
    return res;
}

QString MySQLSQLGenerator::getDropTrigger(const QString& trig)
{
    QString res = "DROP TRIGGER IF EXISTS " + trig;
    return res;

}
