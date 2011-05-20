#ifndef UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define UNARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "WhereExpressionQueryComponent.h"

class CellForUnaryWhereExpression;

/**
 * Class, representing a binary where expression
 */
class UnaryWhereExpressionQueryComponent : public WhereExpressionQueryComponent
{
public:
    UnaryWhereExpressionQueryComponent(QueryComponent*,int);
    virtual QString get();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){};
    virtual QueryComponent* duplicate();

    virtual QSet<CellTypeChooserType>  getTypeset() const
    {
        return m_smallTypes;
    }

private:
    CellForUnaryWhereExpression* m_gritm;
    // the set of types that were activated with the corner cell type chooser (NOT, NEG, etc...)
    QSet<CellTypeChooserType> m_smallTypes;

    // the big type chooser (function, column, etc)
    CellTypeChooserType m_bigType;

};

#endif // UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
