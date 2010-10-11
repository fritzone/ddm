#ifndef MYSQLSQLGENERATOR_H
#define MYSQLSQLGENERATOR_H

#include "AbstractSQLGenerator.h"

class MySQLSQLGenerator : public AbstractSqlGenerator
{
public:

    MySQLSQLGenerator(){}

    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, QString tabName) const;

    virtual QStringList generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const;

};

#endif // MYSQLSQLGENERATOR_H
