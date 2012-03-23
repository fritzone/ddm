#ifndef ABSTRACTSQLGENERATOR_H
#define ABSTRACTSQLGENERATOR_H

#include <QString>
#include <QHash>
#include <QStringList>

class Table;
class TableInstance;
class View;
class StoredMethod;
class Trigger;

/**
 * Interface towards the Database Engine responsible for generating the SQLs for the given database
 * objects.
 */
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
     *
     * @return a list of SQL commands as string values for creating the table, the indexes and the
     * foreign keys for this table.
     *
     */
    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName) const = 0;

    /**
     * Generates a list of "Insert Into" sqls for the given table instance, one for each row in the default values.
     *
     * @param tableInstance - the table instance for which we are generating the SQL
     * @param options - the SQL generation options
     *
     * @return a list of SQL "INSERT INTO" commands for inserting the default values in the given table instance
     */
    virtual QStringList generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const = 0;

    /**
     * Generates a list of "Insert Into" sqls for the given table, one for each row in the default values.
     * This is used when the project is not OOP
     *
     * @param table - the table instance for which we are generating the SQL
     * @param options - the SQL generation options
     *
     * @return a list of SQL "INSERT INTO" commands for inserting the default values in the given table
     */
    virtual QStringList generateDefaultValuesSql(Table* table, const QHash<QString, QString>& options) const = 0;

    /**
     * Generate a "CREATE VIEW" Sql for the given view. If the view is manual, it will simply return
     * the SQL that was typed in by the user.
     *
     * @param v - the view for which we are generating the SQL
     * @param options - the SQL generation options
     *
     * @return a list of SQL commands for creating the view
     */
    virtual QStringList generateCreateViewSql(View* v, const QHash<QString, QString>& options) const = 0;

    /**
     * Generate an "ALTER TABLE" sql for updating the table with the foreign key commands. The foreign key
     * commands were fed back in to the table by the @see generateCreateTableSql in a previous step.
     *
     * @param t - the table for which we are generating the SQL
     * @param options - the SQL generation options
     *
     * @return a list of SQL commands for updating the table with the foreign keys of it.
     */
    virtual QStringList generateAlterTableForForeignKeys(Table* t, const QHash<QString, QString>& options) const = 0;

    /**
     * Generate an SQL script for the given procedure using the options for the script generation. Basically the second
     * part is ignored since the procedures right now are being manually typed in, so this method simply returns
     * the sql script of the procedure.
     *
     * @param p - the procedure
     * @param options - the options
     *
     * @return the statements for creating the procedure
     */
    virtual QStringList generateCreateStoredMethodSql(StoredMethod* p, const QHash<QString, QString>& options) const = 0;

    /**
     * Generate and SQL script for the given trigger. The body of the trigger is written by the database developer
     * but the rest is generated
     *
     * @param t - the trigger to generate the SQL for
     * @param options - the SQL generation options
     *
     * @return a list of SQL commands for generating this trigger
     */
    virtual QStringList generateTriggerSql(Trigger* t, const QHash<QString, QString>& options) const = 0;

    virtual ~AbstractSqlGenerator() {}
};

#endif // ABSTRACTSQLGENERATOR_H
