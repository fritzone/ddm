#ifndef UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define UNARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "WhereExpressionQueryComponent.h"

/**
 * Class, representing a binary where expression
 */
class UnaryWhereExpressionQueryComponent : public WhereExpressionQueryComponent
{
public:
    UnaryWhereExpressionQueryComponent(QueryComponent*,int);
    virtual QString get();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action, QueryComponent* referringObject){};
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){};
    virtual QueryComponent* duplicate();

};

#endif // UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
