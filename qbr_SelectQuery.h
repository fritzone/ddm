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
class Column;

class SelectQuery : public Query
{
public:

    enum NewWhereExpressionType
    {
        PLAIN_NEW_WHERE_EXPRESSION = 0,
        OR_NEW_WHERE_EXPRESSION = 1,
        AND_NEW_WHERE_EXPRESSION = 2
    } ;

    SelectQuery(QueryGraphicsHelper*, int, SqlSourceEntity*);
    SelectQuery(QueryGraphicsHelper*, SqlSourceEntity*);    // called by the deserialization

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
    virtual QVector<const Table*> getTables() const;
    virtual QString get() const;
    virtual QString getClass() const {return "SelectQuery";}
    void newFromTableComponent();
    void newFromSelectQueryComponent();
    void duplicateFromsChild(QueryComponent*);

    void newWhereExpression(SelectQuery::NewWhereExpressionType t = PLAIN_NEW_WHERE_EXPRESSION);
    void newHavingExpression(SelectQuery::NewWhereExpressionType t = PLAIN_NEW_WHERE_EXPRESSION);
    void newSelectExpression();
    void newGroupByExpression();
    void newOrderByExpression();

    void removeFrom();
    bool hasWhere();
    bool hasGroupBy();

    void setSelect(SelectQuerySelectComponent* s)   // called by the deserialization
    {
        m_select = s;
    }
    void setFrom(SelectQueryFromComponent* s)   // called by the deserialization
    {
        m_from = s;
    }
    void setWhere(SelectQueryWhereComponent* s)   // called by the deserialization
    {
        m_where = s;
    }


    bool hasGroupByFunctions();
    bool hasAtLeastOneColumnSelected();
    QVector<const Column*> getSelectedColumns();    // used only for group by stuff, not for the SQL writer ...

    /**
     *  Returns a list of strings that can be used in an order by command. The format of the strings is: $COLUMN ^ALIAS #nr
     *  Based on the following rules:
     *   1. if the item has an alias, it is used its name
     *   2. if the item is a column, but has no alias it is used
     *   3. if the item is an expression and has no alias its order is used.
     */
    QStringList getOrderByElements();
    QVector<const QueryComponent*> getSelectedComponents();
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

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
