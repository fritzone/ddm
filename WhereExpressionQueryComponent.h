#ifndef WHEREEXPRESSIONQUERYCOMPONENT_H
#define WHEREEXPRESSIONQUERYCOMPONENT_H

#include "QueryComponents.h"

/**
 * Base class for a where expression. This according to the MySQL documentation, can be something like:
 * - Unary Expression (!, function, +, -, IS NOT NULL, etc...)
 *   For unary expression the type of the cell can be:
 *    - COLUMN - this by default is a column from a table
 *    - FUNCTION CALL - this is a function call
 *    - ALPHANUMERIC - this is typed in text or numbers
 *    - QUERY - this will create a query as a child of this... uhh...
 *    These above will be accessible in a "big" type chooser, just after the regular type chooser
 *    and the operators: !, -, IS NOT NULL will be acceessible from a classical type chooser.
 *
 * - Binary Expression (Unary OPERATOR Unary, where OPERATOR is Either + - or LIKE (NOT Like))
 * - Ternary Expression (Something BETWEEN A AND B)
 * For now, these will be implemented, since I don't want to create very MySQL specific query builder
 */
class WhereExpressionQueryComponent : public QueryComponent
{
public:
    WhereExpressionQueryComponent(QueryComponent* p, int l):QueryComponent(p,l){}
    virtual QString get() = 0;
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*) = 0;
    virtual void handleAction(const QString& action, QueryComponent* referringObject) = 0;
    virtual QSet<OptionsType> provideOptions() = 0;
    virtual void onClose() = 0;
    virtual QueryComponent* duplicate() = 0;

    QueryGraphicsHelper* getHelper() {return m_helper;}

protected:
    QueryGraphicsHelper* m_helper;

};

#endif // WHEREEXPRESSIONQUERYCOMPONENT_H
