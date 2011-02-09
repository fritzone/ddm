#ifndef SELECTQUERY_H
#define SELECTQUERY_H

#include "Query.h"
#include "SelectQueryGraphicsItem.h"

class SelectQuery : public Query
{
public:
    SelectQuery();

    virtual QueryGraphicsItem* getGraphicsItem();

private:

    SelectQueryGraphicsItem* m_gitm;
};

#endif // SELECTQUERY_H
