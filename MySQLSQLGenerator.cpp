#include "MySQLSQLGenerator.h"
#include "Table.h"
#include "UserDataType.h"
#include "Column.h"
#include "AbstractStorageEngine.h"
#include "Index.h"
#include "ForeignKey.h"

QString MySQLSQLGenerator::generateSql(Table *table, const QHash<QString, QString> &options, QString tabName) const
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

    QString result ;
    if(comments)
    {
        result = "-- ";
        result += (table->getDescription().length()>0?table->getDescription():" Create table " + tabName) + "\n";
    }
    result += upcase? "CREATE " : "create ";
    result += table->isTemporary()? upcase? "TEMPORARY ":"temporary ":"";

    result += !upcase? "table ":" TABLE ";

    // table name
    result += backticks?"`":"";
    result += tabName;
    result += backticks?"`":"";

    result += "\n(\n";

    // creating the columns
    for(int i=0; i<table->fullColumns().size(); i++)
    {
        // column name
        result += backticks?"`":"";
        result += "\t" + table->fullColumns()[i];
        result += backticks?"`":"";
        result += " ";

        Column *col = table->getColumn(table->fullColumns()[i]);
        if(col == 0)
        {
            col = table->getColumnFromParents(table->fullColumns()[i]);
            if(col == 0)
            {
                return "ERROR";
            }
        }

        // column type
        const UserDataType* dt = table->getDataTypeOfColumn(table->fullColumns()[i]);
        result += upcase?dt->sqlAsString().toUpper():dt->sqlAsString().toLower();

        // is this primary key?
        if(col->isPk())
        {
            if(pkpos == 0)
            {
                result += upcase?" PRIMARY KEY ":" primary key ";
            }
            else
            {
                primaryKeys.append(table->fullColumns()[i]);
            }
        }

        // is this a nullable column?
        if(!dt->isNullable())
        {
            result += upcase? " NOT NULL " : " not null ";
        }

        // do we have a default value for this columns' data type?
        if(dt->getDefaultValue().length() > 0)
        {
            result += QString(upcase?" DEFAULT ":" default ") +
                      QString(dt->getType()==DataType::DT_STRING?"\"":"") + dt->getDefaultValue() +
                      QString(dt->getType()==DataType::DT_STRING?"\"":"") ;
        }

        // do we have more columns after this?
        if(i<table->fullColumns().size() - 1)
        {
            result += ",\n";
        }
    }

    // are we having primary keys after columns?
    if(pkpos == 1)
    {
        result += "\n, ";
        result += upcase?"PRIMARY KEY ":"primary key ";
        for(int i=0; i<primaryKeys.size(); i++)
        {
            result += primaryKeys[i];
            if(i<primaryKeys.size() - 1)
            {
                result += ", ";
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
        result += ",\n";
        for(int i=0; i<foreignKeys.size(); i++)
        {
            result += "\t" + foreignKeys[i];
            if(i<foreignKeys.size() - 1)
            {
                result += ",\n";
            }
        }
    }

    // this is closing the crate table SQL
    result += "\n)\n";

    // now check the engine
    if(table->getStorageEngine())
    if(table->getStorageEngine()->name().length() > 0)
    {
        result += QString(upcase?"ENGINE = ":"engine = ") + table->getStorageEngine()->name();
    }
    result += ";\n\n";
    if(comments) result += table->fullIndices().size()>0?"-- Create the indexes for table " + table->getName() + "\n":"";

    // now create the indexes of the table

    for(int i=0; i<table->fullIndices().size(); i++)
    {
        result += upcase?"CREATE INDEX ":"create index ";
        result += table->fullIndices().at(i);
        result += upcase?" USING ":" using ";
        Index* idx = table->getIndex(table->fullIndices().at(i));
        if(idx == 0)
        {
            idx = table->getIndexFromParents(table->fullIndices().at(i));
            if(idx == 0)
            {
                return "ERROR";
            }
        }
        result += upcase?idx->getType().toUpper():idx->getType().toLower();
        result +=upcase?" ON":" on ";
        result += table->getName();
        result += "(";

        for(int j=0; j<idx->getColumns().size(); j++)
        {
            result += idx->getColumns().at(j)->getName();
            if(j<idx->getColumns().size() - 1)
            {
                result += ", ";
            }
        }
        result +=");\n\n";
    }

    // and check if we have foreign keys
    if(fkpos == 2 && foreignKeys.size() > 0)
    {
        if(comments) result += "-- Create the foreign keys for table " + table->getName() + "\n";
        result += upcase?"\nALTER TABLE" + table->getName() :"\nalter table " + table->getName();
        result += upcase?" ADD":" add";
        for(int i=0; i<foreignKeys.size(); i++)
        {
            result += foreignKeys[i];
            if(i<foreignKeys.size() - 1)
            {
                result += ",\n";
            }
        }
    }

    return result;
}
