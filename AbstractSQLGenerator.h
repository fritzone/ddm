#ifndef ABSTRACTSQLGENERATOR_H
#define ABSTRACTSQLGENERATOR_H

#include <QString>
#include <QHash>

class Table;

class AbstractSqlGenerator
{
public:

    /**
     * Generates SQL for the given table, using the given set of options
     */
    virtual QString generateSql(const Table* table, const QHash<QString, QString>& options) = 0;
};

#endif // ABSTRACTSQLGENERATOR_H
