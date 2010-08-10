#ifndef MYSQLSQLGENERATOR_H
#define MYSQLSQLGENERATOR_H

#include "AbstractSQLGenerator.h"

class MySQLSQLGenerator : public AbstractSqlGenerator
{
public:

    MySQLSQLGenerator(){}

    virtual QStringList generateSql(Table* table, const QHash<QString, QString>& options, QString tabName) const;
};

#endif // MYSQLSQLGENERATOR_H
