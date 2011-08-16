#ifndef SELECTQUERY_H
#define SELECTQUERY_H

#include "qbr_Query.h"
#include "qbr_SelectQueryGraphicsItem.h"
#include "qbr_QueryGraphicsHelper.h"

class QueryComponent;
class SelectQuerySelectComponent;
class SelectQueryFromComponent;
class SelectQueryWhereComponent;
class SelectQueryGroupByComponent;
class SelectQueryAsComponent;
class SelectQueryOrderByComponent;

class SelectQuery : public Query
{
public:
    SelectQuery(QueryGraphicsHelper*, int);
    virtual bool initializeGraphicsItem();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();
    virtual QueryComponent* duplicate();
    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }

    void newFromTableComponent();
    void newFromSelectQueryComponent();
    void duplicateFromsChild(QueryComponent*);

    void newWhereExpression();

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
    SelectQueryOrderByComponent* m_orderBy;
};

#endif // SELECTQUERY_H
