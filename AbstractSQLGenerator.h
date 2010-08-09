#ifndef ABSTRACTSQLGENERATOR_H
#define ABSTRACTSQLGENERATOR_H

#include <QString>
#include <QHash>

class Table;

class AbstractSqlGenerator
{
public:

    /**
     * Generates SQL for the given table, using the given set of options. The table name although should be tabName
     */

    TODO: rewrite this and the implementation so, that it returns a QVector<QString>

    virtual QString generateSql(Table* table, const QHash<QString, QString>& options, QString tabName) const = 0;
};

#endif // ABSTRACTSQLGENERATOR_H
