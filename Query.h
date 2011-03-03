#ifndef QUERY_H
#define QUERY_H

#include "QueryGraphicsItem.h"
#include "QueryComponents.h"

class QueryGraphicsHelper;


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

    QueryGraphicsHelper* getHelper()
    {
        return m_helper;
    }

    virtual QString get()
    {
        return "CREATE THE SQL";
    }

protected:
    QueryGraphicsHelper* m_helper;
    QueryGraphicsItem* m_graphicsItem;
};

#endif // QUERY_H
