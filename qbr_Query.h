#ifndef QUERY_H
#define QUERY_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryComponents.h"

class QueryGraphicsHelper;
class Table;

class Query : public QueryComponent
{
public:

    Query(QueryGraphicsHelper*,int);
    virtual ~Query() {}

    QueryGraphicsItem* getGraphicsItem()
    {
        return m_graphicsItem;
    }

    virtual bool initializeGraphicsItem() = 0;
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*) = 0;
    virtual QVector<const Table*> getTables() const = 0;

    QueryGraphicsHelper* getHelper()
    {
        return m_helper;
    }

    virtual QString get() const = 0;

protected:
    QueryGraphicsHelper* m_helper;
    QueryGraphicsItem* m_graphicsItem;
};

#endif // QUERY_H
