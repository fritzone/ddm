#ifndef ABSTRACTSQLGENERATOR_H
#define ABSTRACTSQLGENERATOR_H

#include <QString>
#include <QHash>

class Table;
class TableInstance;

class AbstractSqlGenerator
{
public:

    /**
     * Generates a "Create Table" SQL for the given table, using the given set of options.
     * The generated table name should be tabName because this method is used in the table
     * instances generation too. Should generate the "Alter Table" commands too, but only if
     * the foreign keys are asked for in the final sql generation, feeds back these alter
     * tables to the table parameter, that's why the table parameter must be non const.
     * For most of the cases the options parameter will be taken over from the Configuration
     * but also there is a possibility for manually creating the object and sending it over here.
     * For the usage of the options @see Configuration
     *
     * @param table - the table for which the SQL is being generated.
     * @param options - the options that are used for code generation
     * @param tabName - this is the name of the table that will be used in the code generation
     * @param codepage - this is the codepage
     *
     * @return a list of SQL commands as string values for creating the table, the indexes and the
     * foreign keys for this table.
     *
     */
    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName, const QString& codepage) const = 0;

    /**
     * Generates a list of "Insert Into" sqls for the given table instance, one for each row in the default values.
     */
    virtual QStringList generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const = 0;

    /**
     * Generates a list of "Insert Into" sqls for the given table, one for each row in the default values.
     * This is used when the project is not OOP
     */
    virtual QStringList generateDefaultValuesSql(Table* table, const QHash<QString, QString>& options) const = 0;

    virtual ~AbstractSqlGenerator() {}
};

#endif // ABSTRACTSQLGENERATOR_H
