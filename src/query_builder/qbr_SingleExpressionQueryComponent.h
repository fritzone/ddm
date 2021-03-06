#ifndef UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
#define UNARYWHEREEXPRESSIONQUERYCOMPONENT_H

#include "qbr_QueryComponents.h"

#include <QMap>

class CellForSingleExpression;
class CellTypeChooser;
class DatabaseFunctionInstantiationComponent;
class DatabaseBuiltinFunction;
class Column;
class SelectQueryJoinComponent;
class SelectQueryAsComponent;
class Table;
class TableInstance;

struct ColumnOfTabWithTabInstance
{
    Table* tab;
    TableInstance* tinst;
    Column* c;
};

/**
 * Class, representing a unary where expression.
 * On an initial startup the Unary query component is a simple column chooser
 */
class SingleExpressionQueryComponent : public QueryComponent
{
public:

    enum ForcedSingleExpressionQueryComponent
    {
        FORCED_OR = 0,
        FORCED_AND = 1,
        NOT_FORCED = 2
    };

    SingleExpressionQueryComponent(Query *q, QueryComponent*, int, Version *v);
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
    const ColumnOfTabWithTabInstance *getColumnAt(int i);
    const QString getTypedInValueAt(int i);

    DatabaseFunctionInstantiationComponent* getFunctionInstantiationAt(int);
    void setForcedType(ForcedSingleExpressionQueryComponent, bool fromDeserialization=false);
    bool hasForcedType() const
    {
        return m_forcedType == FORCED_AND || m_forcedType == FORCED_OR;
    }

    void setOwnedByOn(SelectQueryJoinComponent* onOwner, SingleExpressionQueryComponent* onComponent)
    {
        m_ownedByOn = true;
        m_onOwner = onOwner;
        m_onComponent = onComponent;
    }

    void removeFromOn();
    bool hasGroupByFunctions();
    bool hasAtLeastOneColumnSelected();

    QVector<const ColumnOfTabWithTabInstance *> getColumns();
    void removeAs();
    const SelectQueryAsComponent* hasAs();
    bool hasStar();
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    void setColumnAtGivenPosition(int pos, ColumnOfTabWithTabInstance* col)
    {
        m_columnsAtGivenPosition[pos] = col;
    }

    void setTextAtGivenPosition(int pos, const QString& s)
    {
        m_typedValuesAtGivenPosition[pos] = s;
    }

    void addElement(CellTypeChooserType a)
    {
        m_elements.push_back(a);
    }

    void setFunctionInstantiationAtGivenPosition(int p, DatabaseFunctionInstantiationComponent* c, const DatabaseBuiltinFunction* f)
    {
        m_functionInstantiationAtGivenPosition[p] = c;
        m_functionsAtGivenPosition[p] = f;
    }

    void setAs(SelectQueryAsComponent* as)
    {
        m_as = as;
    }

    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
private:

    QMap<CellTypeChooserType,QString> prepareMappings() const;

private:

    QueryGraphicsHelper* m_helper;
    // the graphic element
    CellForSingleExpression* m_gritm;

    // the list of elements that are in the expression
    QVector<CellTypeChooserType> m_elements;

    // if at a given position there is a column, it is put in this map
    QMap<int, const ColumnOfTabWithTabInstance*> m_columnsAtGivenPosition;

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
    SingleExpressionQueryComponent* m_onComponent;

    // in case this has an ALIAS
    SelectQueryAsComponent* m_as;
    bool m_hasForcedType;
};

#endif // UNARYWHEREEXPRESSIONQUERYCOMPONENT_H
