#ifndef UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define UNARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "qbr_WhereExpressionQueryComponent.h"
#include "DatabaseBuiltinFunction.h"
#include "qbr_DatabaseFunctionInstantiationComponent.h"

class CellForUnaryWhereExpression;
class CellTypeChooser;

/**
 * Class, representing a unary where expression.
 * On an initial startup the Unary query component is a simple column chooser
 */
class SingleExpressionQueryComponent : public WhereExpressionQueryComponent
{
public:
    SingleExpressionQueryComponent(QueryComponent*,int);
    virtual QString get();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}

    virtual QueryComponent* duplicate();

    virtual QVector<CellTypeChooserType> getFunctionsAndOperators() const
    {
        return m_functionsAndOperators;
    }

    QVector<CellTypeChooserType> getChoosableTypes() const;

    bool hasFunctionAtIndex(int);
    DatabaseBuiltinFunction* getFunctionAt(int i)
    {
        return &m_functionsAtGivenPosition[i];
    }
    void shiftFunctionsToTheLeft(int after);

private:
    CellForUnaryWhereExpression* m_gritm;
    // the set of types that were activated with the corner cell type chooser (NOT, NEG, etc...)
    QVector<CellTypeChooserType> m_functionsAndOperators;

    QMap<int, DatabaseBuiltinFunction> m_functionsAtGivenPosition;
    QMap<int, DatabaseFunctionInstantiationComponent> m_functionInstantiationAtGivenPosition;


};

#endif // UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
