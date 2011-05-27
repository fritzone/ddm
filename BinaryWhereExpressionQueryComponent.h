#ifndef BINARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define BINARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "WhereExpressionQueryComponent.h"

class CellForBinaryWhereExpression;
class QueryGraphicsHelper;

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
    virtual QSet<CellTypeChooserType>  getTypeset() const
    {
        return QSet<CellTypeChooserType> ();
    }

    /** creates the left and the right side, so that it does not fall into infinite recursion */
    void createSides();
    WhereExpressionQueryComponent* left() {return m_left;}
    WhereExpressionQueryComponent* right() {return m_right;}
    CellTypeChooserType getOperation()
    {
        return m_operation;
    }

private:

    // initially, these two below are unary components. Unary components can be transformed into binary
    WhereExpressionQueryComponent* m_left;
    WhereExpressionQueryComponent* m_right;

    // the graphics item for the component
    CellForBinaryWhereExpression* m_gritm;
    CellTypeChooserType m_operation;


};
#endif // BINARYWHEREEXPRESSIONQUERYCOMPONENT_H
