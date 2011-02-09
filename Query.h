#ifndef QUERY_H
#define QUERY_H

#include "QueryGraphicsItem.h"

class Query
{
public:
    Query();

    virtual QueryGraphicsItem* getGraphicsItem() = 0;
};

#endif // QUERY_H
