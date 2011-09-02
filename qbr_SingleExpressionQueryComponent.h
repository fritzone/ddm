#ifndef UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define UNARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "qbr_WhereExpressionQueryComponent.h"

#include <QMap>

class CellForSingleExpression;
class CellTypeChooser;
class DatabaseFunctionInstantiationComponent;
class DatabaseBuiltinFunction;
class Column;

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
        return m_elements;
    }

    QVector<CellTypeChooserType> getChoosableTypes() const;

    bool hasFunctionAtIndex(int);
    const DatabaseBuiltinFunction* getFunctionAt(int i);
    void shiftElementsToTheLeft(int after);
    const Column* getColumnAt(int i);
    const QString& getTypedInValueAt(int i);

    DatabaseFunctionInstantiationComponent* getFunctionInstantiationAt(int);

private:

    // the graphic element
    CellForSingleExpression* m_gritm;

    // the list of elements that are in the expression
    QVector<CellTypeChooserType> m_elements;

    // if at a given position there is a column, it is put in this map
    QMap<int, const Column*> m_columnsAtGivenPosition;

    // if at a given position there is a function it is put in this map
    QMap<int, const DatabaseBuiltinFunction*> m_functionsAtGivenPosition;

    // if at a given position there is a function then at the given position must be a function instantiation too
    QMap<int, DatabaseFunctionInstantiationComponent*> m_functionInstantiationAtGivenPosition;

    // if at a given position there is a typed in value it is put in this map
    QMap<int, QString> m_typedValuesAtGivenPosition;

};

#endif // UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
