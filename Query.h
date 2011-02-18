#ifndef QUERY_H
#define QUERY_H

#include "QueryGraphicsItem.h"

class QueryGraphicsHelper;

class Query
{
public:
    Query(QueryGraphicsHelper*);

    QueryGraphicsItem* getGraphicsItem()
    {
        return m_graphicsItem;
    }

    virtual bool createGraphicsItem() = 0;

    QueryGraphicsHelper* getHelper()
    {
        return m_helper;
    }


protected:
    QueryGraphicsHelper* m_helper;
    QueryGraphicsItem* m_graphicsItem;
};

#endif // QUERY_H
