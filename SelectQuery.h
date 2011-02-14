#ifndef SELECTQUERY_H
#define SELECTQUERY_H

#include "Query.h"
#include "SelectQueryGraphicsItem.h"
#include "QueryComponents.h"

class SelectQuery : public Query
{
public:
    SelectQuery(QueryComponents*);

    virtual QueryGraphicsItem* getGraphicsItem();

private:

    SelectQueryGraphicsItem* m_gitm;
    QueryComponents* m_components;
};

#endif // SELECTQUERY_H
