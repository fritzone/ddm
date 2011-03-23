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
     * Generates a "Create Table" SQL for the given table, using the given set of options. The geenrated table name should be tabName
     * because this method is used in the table instances generation too. Should generate the "Alter Table" commands too, but only if
     * the foreign keys are asked for in the final sql generation, feeds back these alter tables to the table parameter.
     *
     * The options are like:
     * options["Case"] == "Upper" | "Lower" - whether the output code will be upper case or lower case
     * options["GenerateComments"] == "Yes" | "No" - whether to generate any comment in the outgoing code
     * options["Backticks"] == "Yes" | "No" - whether to add backticks in the `names`
     * options["PKSposition"]=="ColumnDeclaration" | "AfterColumnsDeclaration" | "AfterTableDeclaration"
     *                          - where to put the primary keys: in the column declaration itself | after the columns declaration | after the table declaration in an ALTER command
     *
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
};

#endif // ABSTRACTSQLGENERATOR_H
