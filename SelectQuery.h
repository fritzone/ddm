#ifndef SELECTQUERY_H
#define SELECTQUERY_H

#include "Query.h"
#include "SelectQueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"

class QueryComponent;
class SelectQuerySelectComponent;
class SelectQueryFromComponent;
class SelectQueryWhereComponent;
class SelectQueryGroupByComponent;
class SelectQueryAsComponent;

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

    void removeFrom();
    bool hasWhere();
    bool hasGroupBy();

    bool hasGroupByFunctions();

private:
    SelectQuerySelectComponent* m_select;
    SelectQueryFromComponent* m_from;
    SelectQueryWhereComponent* m_where;
    SelectQueryGroupByComponent* m_groupby;
    SelectQueryWhereComponent* m_having;
    SelectQueryAsComponent* m_as;
};

#endif // SELECTQUERY_H
