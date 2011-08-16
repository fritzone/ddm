#ifndef UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define UNARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "qbr_WhereExpressionQueryComponent.h"

#include <QMap>

class CellForSingleExpression;
class CellTypeChooser;
class DatabaseFunctionInstantiationComponent;
class DatabaseBuiltinFunction;

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
    const DatabaseBuiltinFunction* getFunctionAt(int i);
    void shiftFunctionsToTheLeft(int after);

    DatabaseFunctionInstantiationComponent* getFunctionInstantiationAt(int);

private:
    CellForSingleExpression* m_gritm;
    // the set of types that were activated with the corner cell type chooser (NOT, NEG, etc...)
    QVector<CellTypeChooserType> m_functionsAndOperators;

    QMap<int, const DatabaseBuiltinFunction*> m_functionsAtGivenPosition;
    QMap<int, DatabaseFunctionInstantiationComponent*> m_functionInstantiationAtGivenPosition;


};

#endif // UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
