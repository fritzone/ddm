#ifndef SELECTQUERY_H
#define SELECTQUERY_H

#include "Query.h"
#include "SelectQueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"

class QueryComponent;
class SelectQuerySelectComponent;
class SelectQueryFromComponent;

class SelectQuery : public Query
{
public:
    SelectQuery(QueryGraphicsHelper*);
    virtual bool createGraphicsItem();
    void newFromComponent();

private:
    SelectQuerySelectComponent* m_select;
    SelectQueryFromComponent* m_from;
    //QueryComponent* m_where;
};

#endif // SELECTQUERY_H
