#ifndef MYSQLSQLGENERATOR_H
#define MYSQLSQLGENERATOR_H

#include "db_AbstractSQLGenerator.h"

class MySQLSQLGenerator : public AbstractSqlGenerator
{
public:

    MySQLSQLGenerator(){}
    ~MySQLSQLGenerator() {}

    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName, const QString& codepage) const;
    virtual QStringList generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const;
    virtual QStringList generateDefaultValuesSql(Table* table, const QHash<QString, QString>& options) const;
    virtual QStringList generateCreateViewSql(View *v, const QHash<QString, QString> &options) const;
    virtual QStringList generateAlterTableForForeignKeys(Table* t, const QHash<QString, QString>& options) const;
    virtual QStringList generateCreateProcedureSql(Procedure* p, const QHash<QString, QString>& options) const;

private:

    QString quotelessString(const QString&) const;

};

#endif // MYSQLSQLGENERATOR_H
