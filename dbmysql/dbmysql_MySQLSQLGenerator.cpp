#include "dbmysql_MySQLSQLGenerator.h"
#include "Table.h"
#include "UserDataType.h"
#include "Column.h"
#include "db_AbstractStorageEngine.h"
#include "Index.h"
#include "ForeignKey.h"
#include "TableInstance.h"
#include "core_View.h"
#include "qbr_SelectQuery.h"
#include "core_Procedure.h"
#include "core_Trigger.h"

QStringList MySQLSQLGenerator::generateCreateTableSql(Table *table, const QHash<QString, QString> &options, const QString& tabName, const QString& codepage) const
{
    // do not generate any code for a table which has no columns
    if(table->fullColumns().size() == 0) return QStringList();

    bool upcase = options.contains("Case") && options["Case"] == "Upper";
    bool comments = options.contains("GenerateComments") && options["GenerateComments"] == "Yes";
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
    int fkpos = 1;
    if(options.contains("FKSposition"))
    {
        if(options["FKSposition"] == "InTable")
        {
            fkpos = 1;
        }
        if(options["FKSposition"] == "AfterTable")
        {
            fkpos = 2;
        }
        if(options["FKSposition"] == "OnlyInternal")
        {
            fkpos = 3;
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
    createTable += table->isTemporary()? upcase? "TEMPORARY ":"temporary ":"";

    createTable += !upcase? "table ":"TABLE ";

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

        createTable += "\t";
        createTable += (backticks?"`":"") + table->fullColumns()[i];
        createTable += backticks?"`":"";
        createTable += " ";

        Column *col = table->getColumn(table->fullColumns()[i]);
        if(col == 0)
        {
            col = table->getColumnFromParents(table->fullColumns()[i]);
            if(col == 0)
            {
                return QStringList("ERROR");
            }
        }

        // column type
        const UserDataType* dt = table->getDataTypeOfColumn(table->fullColumns()[i]);
        createTable += upcase?dt->sqlAsString().toUpper():dt->sqlAsString().toLower();

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

        // do we have a default value for this columns' data type?
        if(dt->getDefaultValue().length() > 0)
        {
            QString g = dt->getDefaultValue();
            createTable += QString(upcase?" DEFAULT ":" default ") +
                    QString( (dt->getType()==DataType::DT_STRING) || (dt->getType()==DataType::DT_DATETIME && g.toUpper()!="CURRENT_TIMESTAMP") ? "\"" : "") + dt->getDefaultValue() +
                    QString( (dt->getType()==DataType::DT_STRING) || (dt->getType()==DataType::DT_DATETIME && g.toUpper()!="CURRENT_TIMESTAMP") ? "\"" : "") ;
        }

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
    for(int i=0; i<table->getFks().size(); i++)
    {
        // just pre-render the SQL for foreign keys
        QString foreignKeySql1 = "";
        QString foreignKeySql2 = "";

        ForeignKey* fkI = table->getFks().at(i);
        foreignKeysTable = fkI->getForeignTableName();
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
        QString foreignKeySql = upcase?"FOREIGN KEY (":"foreign key(";
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
    if(fkpos == 1 && foreignKeys.size() > 0)
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

    // now check the engine
    if(table->getStorageEngine())
    if(table->getStorageEngine()->name().length() > 0)
    {
        createTable += QString(upcase?"ENGINE = ":"engine = ") + table->getStorageEngine()->name();
    }

    // and the codepage
    if(codepage.length() > 1)
    {
        createTable += upcase?" DEFAULT CHARACTER SET " + codepage:" default character set " +codepage;
    }
    else
    {
        createTable += " DEFAULT CHARACTER SET=latin1" ;
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
        QString indexCommand = upcase?"CREATE INDEX ":"create index ";
        indexCommand += table->fullIndices().at(i);
        indexCommand += upcase?" USING ":" using ";
        Index* idx = table->getIndex(table->fullIndices().at(i));
        if(idx == 0)
        {
            idx = table->getIndexFromParents(table->fullIndices().at(i));
            if(idx == 0)
            {
                return QStringList("ERROR");
            }
        }
        indexCommand += upcase?idx->getType().toUpper():idx->getType().toLower();
        indexCommand +=upcase?" ON ":" on ";
        indexCommand += tabName;
        indexCommand += "(";

        for(int j=0; j<idx->getColumns().size(); j++)
        {
            indexCommand += idx->getColumns().at(j)->getName();
            if(j<idx->getColumns().size() - 1)
            {
                indexCommand += ", ";
            }
        }
        indexCommand +=");\n\n";

        toReturn << indexCommand;
    }

    // and check if we have foreign keys
    if(fkpos == 2 && foreignKeys.size() > 0)
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
            insert += tableInstance->columns().at(j);
            if(j< tableInstance->columns().size() - 1)
            {
                insert += ", ";
            }

        }
        insert += upcase?" ) VALUES (" : ") values (";
        for(int j=0; j<tableInstance->columns().size(); j++)
        {
            QVector<QString> vals = tableInstance->values()[tableInstance->columns().at(j)];
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
        insert += ");\n\n";
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

QStringList MySQLSQLGenerator::generateCreateViewSql(View *v, const QHash<QString, QString> &options) const
{
    if(v->getManual())
    {
        QStringList res;
        res.append(v->getManualSql());
        return res;
    }
    else
    {
        QStringList res;
        bool upcase = options.contains("Case") && options["Case"] == "Upper";
        res.append(upcase?"CREATE ":"create ");
        if(v->getReplace())
        {
            res.append(upcase?"OR REPLACE ":"or replace ");
        }
        res.append(QString(upcase?"VIEW ":"view ") + v->getName());
        if(v->getColumnNames().size() > 0)
        {
            res.append(" (");
            QString c = "";
            for(int i=0; i<v->getColumnNames().size(); i++)
            {
                c += v->getColumnNames().at(i);
                if(i<v->getColumnNames().size() - 1) c += ", ";
            }
            res.append(c);
            res.append(")");
        }
        res.append("\n");
        res.append(upcase?"AS\n":"as\n");
        res.append(v->getQuery()->get());
        QString g;
        for(int i=0; i< res.size(); i++)
        {
            g += res.at(i) + " ";
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

QStringList MySQLSQLGenerator::generateCreateProcedureSql(Procedure *p, const QHash<QString, QString>& /*options*/) const
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
    s += t->getTable()->getName(); s+= strSpace;
    s += upcase? "FOR EACH ROW ":"for each row ";
    s += strNewline + t->getSql() + strSemicolon + strNewline;
    result << s;
    return result;
}