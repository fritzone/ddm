#ifndef BINARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define BINARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "WhereExpressionQueryComponent.h"

/**
 * Class, representing a binary where expression
 */
class BinaryWhereExpressionQueryComponent : public WhereExpressionQueryComponent
{
public:
    BinaryWhereExpressionQueryComponent(QueryComponent*,int);
    virtual QString get();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action, QueryComponent* referringObject){};
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){};
    virtual QueryComponent* duplicate();

    /** creates the left and the right side, so that it does not fall into infinite recursion */
    void createSides();

private:

    // initially, these two below are unary components. Unary components can be transformed into binary
    WhereExpressionQueryComponent* m_left;
    WhereExpressionQueryComponent* m_right;

};
#endif // BINARYWHEREEXPRESSIONQUERYCOMPONENT_H
