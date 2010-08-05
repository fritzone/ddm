#ifndef MYSQLSQLGENERATOR_H
#define MYSQLSQLGENERATOR_H

#include "AbstractSQLGenerator.h"

class MySQLSQLGenerator : public AbstractSqlGenerator
{
public:

    MySQLSQLGenerator(){}

    virtual QString generateSql(const Table* table, const QHash<QString, QString>& options);
};

#endif // MYSQLSQLGENERATOR_H
