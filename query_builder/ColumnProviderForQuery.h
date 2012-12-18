#ifndef COLUMNPROVIDERFORQUERY_H
#define COLUMNPROVIDERFORQUERY_H

#include <QVector>

class Column;

/**
 * This class provides columns for a query.
 */
class ColumnProviderForQuery
{
public:
    virtual QVector<const Column*> provideColumns() = 0;
};

#endif // COLUMNPROVIDERFORQUERY_H
