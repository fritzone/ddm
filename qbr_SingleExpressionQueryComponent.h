#ifndef UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define UNARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "qbr_WhereExpressionQueryComponent.h"

#include <QMap>

class CellForSingleExpression;
class CellTypeChooser;
class DatabaseFunctionInstantiationComponent;
class DatabaseBuiltinFunction;
class Column;
class SelectQueryJoinComponent;
class SelectQueryAsComponent;

/**
 * Class, representing a unary where expression.
 * On an initial startup the Unary query component is a simple column chooser
 */
class SingleExpressionQueryComponent : public WhereExpressionQueryComponent
{
public:

    enum ForcedSingleExpressionQueryComponent
    {
        FORCED_OR = 0,
        FORCED_AND = 1
    };

    SingleExpressionQueryComponent(QueryComponent*,int);
    virtual QString get() const;
    virtual QString getClass() const {return "SingleExpressionQueryComponent";}
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();

    virtual QueryComponent* duplicate();

    virtual QVector<CellTypeChooserType> getElements() const
    {
        return m_elements;
    }

    QVector<CellTypeChooserType> getChoosableTypes() const;

    bool hasFunctionAtIndex(int);
    const DatabaseBuiltinFunction* getFunctionAt(int i);
    void shiftElementsToTheLeft(int after);
    const Column* getColumnAt(int i);
    const QString getTypedInValueAt(int i);

    DatabaseFunctionInstantiationComponent* getFunctionInstantiationAt(int);
    void setForcedType(ForcedSingleExpressionQueryComponent);

    void setOwnedByOn(SelectQueryJoinComponent* onOwner, WhereExpressionQueryComponent* onComponent)
    {
        m_ownedByOn = true;
        m_onOwner = onOwner;
        m_onComponent = onComponent;
    }

    void removeFromOn();
    bool hasGroupByFunctions();
    bool hasAtLeastOneColumnSelected();

    QVector<const Column*> getColumns();
    void removeAs();
    const SelectQueryAsComponent* hasAs();
    bool hasStar();
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

private:

    QMap<CellTypeChooserType,QString> prepareMappings() const;

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

    ForcedSingleExpressionQueryComponent m_forcedType;

    // true if this is owned by an ON query component, in this case
    bool m_ownedByOn;
    SelectQueryJoinComponent* m_onOwner;
    WhereExpressionQueryComponent* m_onComponent;

    // in case this has an ALIAS
    SelectQueryAsComponent* m_as;
};

#endif // UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
