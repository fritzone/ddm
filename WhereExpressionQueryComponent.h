#ifndef WHEREEXPRESSIONQUERYCOMPONENT_H
#define WHEREEXPRESSIONQUERYCOMPONENT_H

#include "QueryComponents.h"

/**
 * Base class for a where expression. This according to the MySQL documentation, can be something like:
 * - Unary Expression (!, function, +, -, IS NOT NULL, etc...)
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
};

#endif // WHEREEXPRESSIONQUERYCOMPONENT_H
