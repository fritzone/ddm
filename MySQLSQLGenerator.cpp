#include "MySQLSQLGenerator.h"
#include "Table.h"
#include "UserDataType.h"
#include "Column.h"
#include "AbstractStorageEngine.h"
#include "Index.h"
#include "ForeignKey.h"
#include "TableInstance.h"

QStringList MySQLSQLGenerator::generateCreateTableSql(Table *table, const QHash<QString, QString> &options, QString tabName) const
{
    bool upcase = options.contains("Case") && options["Case"] == "Upper";
    bool comments = options.contains("GenerateComments") && options["GenerateComments"] == "Yes";
    bool backticks = options.contains("Backticks") && options["Backticks"] == "Yes";
    // primary key pos = 0 if the primary key is specified in the column declaration (default)
    // it is 1 if the primary key is defined in a PRIMARY KEY section in the table definition
    // it is 2 if the primary key is defined in an alter table section after the table definition
    int pkpos = 0;
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
        QString comment = "-- ";
        comment += (table->getDescription().length()>0?table->getDescription():" Create table " + tabName) + "\n";
        toReturn << comment;
    }
    QString createTable = upcase? "CREATE " : "create ";
    createTable += table->isTemporary()? upcase? "TEMPORARY ":"temporary ":"";

    createTable += !upcase? "table ":" TABLE ";

    // table name
    createTable += backticks?"`":"";
    createTable += tabName;
    createTable += backticks?"`":"";

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
            else
            {
                primaryKeys.append(table->fullColumns()[i]);
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
            createTable += QString(upcase?" DEFAULT ":" default ") +
                      QString(dt->getType()==DataType::DT_STRING?"\"":"") + dt->getDefaultValue() +
                      QString(dt->getType()==DataType::DT_STRING?"\"":"") ;
        }

        // do we have more columns after this?
        if(i<table->fullColumns().size() - 1)
        {
            createTable += ",\n";
        }
    }

    // are we having primary keys after columns?
    if(pkpos == 1)
    {
        createTable += "\n, ";
        createTable += upcase?"PRIMARY KEY ":"primary key ";
        for(int i=0; i<primaryKeys.size(); i++)
        {
            createTable += primaryKeys[i];
            if(i<primaryKeys.size() - 1)
            {
                createTable += ", ";
            }
        }
    }


    // creating the FOREIGN KEY sql(s)...
    for(int i=0; i<table->getFks().size(); i++)
    {
        // just pre-render the SQL for foreign keys
        QString foreignKeySql1 = "";
        QString foreignKeySql2 = "";

        ForeignKey* fkI = table->getFks().at(i);
        foreignKeysTable = fkI->getForeignTable();
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
            insert += "\"";
            insert += vals.at(i);
            insert += "\"";
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
