#include "MySQLSQLGenerator.h"
#include "Table.h"
#include "UserDataType.h"
#include "Column.h"
#include "AbstractStorageEngine.h"
#include "Index.h"

QString MySQLSQLGenerator::generateSql(const Table *table, const QHash<QString, QString> &options)
{
    bool upcase = options.contains("Case") && options["Case"] == "Upper";
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

    // the list of primary key columns, used only if pkpos is 1 or 2
    QStringList primaryKeys;

    QString result ;
    result = "-- ";
    result += table->getDescription() + "\n";
    result += upcase? "CREATE " : "create ";
    result += table->isTemporary()? upcase? "TEMPORARY ":"temporary ":"";

    result += !upcase? "table ":" TABLE ";

    // table name
    result += backticks?"`":"";
    result += table->getName();
    result += backticks?"`":"";

    result += "\n(\n";

    for(int i=0; i<table->fullColumns().size(); i++)
    {
        // column name
        result += backticks?"`":"";
        result += "\t" + table->fullColumns()[i];
        result += backticks?"`":"";
        result += " ";

        Column *col = table->getColumn(table->fullColumns()[i]);
        if(col ==0)
        {
            col = table->getColumnFromParents(table->fullColumns()[i]);
            if(col == 0)
            {
                return "ERROR";
            }
        }

        // column type
        const UserDataType* dt = table->getDataTypeOfColumn(table->fullColumns()[i]);
        result += dt->getSqlType();

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
            result += QString(upcase?" DEFAULT ":" default ") + dt->getDefaultValue();
        }

        // do we have more columns after this?
        if(i<table->fullColumns().size() - 1)
        {
            result += ",\n";
        }
    }

    // are we havin primary keys after columns?
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

    result += "\n)\n";

    // now check the engine
    if(table->getStorageEngine())
    if(table->getStorageEngine()->name().length() > 0)
    {
        result += QString(upcase?"ENGINE = ":"engine = ") + table->getStorageEngine()->name();
    }
    result += ";\n\n";

    // now create the indexes of the table

    fix the next stuff, add a new field to Table.h to get index from parents
    for(int i=0; i<table->fullIndices().size(); i++)
    {
        result += upcase?"CREATE INDEX ":"create index ";
        result += table->fullIndices().at(i);
        result +=upcase?" ON":" on ";
        result += table->getName();
        result += "(";
        for(int j=0; j<table->getIndices().at(i)->getColumns().size(); j++)
        {
            result += table->getIndices().at(i)->getColumns().at(j)->getName();
            if(j<table->getIndices().at(i)->getColumns().size() - 1)
            {
                result += ", ";
            }
        }
        result +=");\n";
    }

    return result;
}
