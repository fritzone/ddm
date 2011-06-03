#ifndef UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define UNARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "WhereExpressionQueryComponent.h"

class CellForUnaryWhereExpression;

/**
 * Class, representing a unary where expression.
 * On an initial startup the Unary query component is a simple column chooser
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

    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return m_smallTypes;
    }

    QVector<CellTypeChooserType> getChoosableTypes() const;

private:
    CellForUnaryWhereExpression* m_gritm;
    // the set of types that were activated with the corner cell type chooser (NOT, NEG, etc...)
    QVector<CellTypeChooserType> m_smallTypes;

    // the big type chooser (function, column, etc)
    CellTypeChooserType m_currentBigType;
};

#endif // UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
