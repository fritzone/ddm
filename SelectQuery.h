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
    SelectQuery(QueryGraphicsHelper*, int);
    virtual bool initializeGraphicsItem();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();

    void newFromTableComponent();
    void newFromSelectQueryComponent();
private:
    SelectQuerySelectComponent* m_select;
    SelectQueryFromComponent* m_from;
    SelectQueryWhereComponent* m_where;
};

#endif // SELECTQUERY_H
