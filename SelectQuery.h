#ifndef SELECTQUERY_H
#define SELECTQUERY_H

#include "Query.h"
#include "SelectQueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"

class QueryComponent;
class SelectQuerySelectComponent;
class SelectQueryFromComponent;
class SelectQueryWhereComponent;

class SelectQuery : public Query
{
public:
    SelectQuery(QueryGraphicsHelper*);
    virtual bool initializeGraphicsItem();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    void newFromTableComponent();
    virtual void handleAction(const QString& action);
    virtual QSet<OptionsType> provideOptions();

private:
    SelectQuerySelectComponent* m_select;
    SelectQueryFromComponent* m_from;
    SelectQueryWhereComponent* m_where;
};

#endif // SELECTQUERY_H
