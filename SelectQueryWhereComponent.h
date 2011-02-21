#ifndef SELECTQUERYWHERECOMPONENT_H
#define SELECTQUERYWHERECOMPONENT_H

#include "QueryComponents.h"

class SelectQueryWhereComponent : public QueryComponent
{
public:
    SelectQueryWhereComponent(QueryComponent*);
    virtual QString get(){return "WHERE"; }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
};

#endif // SELECTQUERYWHERECOMPONENT_H
