#include "UnaryWhereExpressionQueryComponent.h"
#include "CellForUnaryWhereExpression.h"

UnaryWhereExpressionQueryComponent::UnaryWhereExpressionQueryComponent(QueryComponent* p, int l): WhereExpressionQueryComponent(p,l), m_smallTypes()
{
}

QueryGraphicsItem* UnaryWhereExpressionQueryComponent::createGraphicsItem(QueryGraphicsHelper * helper, QueryGraphicsItem * parent)
{
    m_helper = helper;
    m_gritm = new CellForUnaryWhereExpression(getLevel() + 1, helper, parent, this);

    return m_gritm;

}

QString UnaryWhereExpressionQueryComponent::get()
{
    return "Implement this";
}

QSet<OptionsType> UnaryWhereExpressionQueryComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

QueryComponent* UnaryWhereExpressionQueryComponent::duplicate()
{
    UnaryWhereExpressionQueryComponent* newc = new UnaryWhereExpressionQueryComponent(m_parent, m_level);
    return newc;
}

void UnaryWhereExpressionQueryComponent::handleAction(const QString& action, QueryComponent* referringObject)
{
    if(action == "NOT")
    {
        m_smallTypes.insert(0, CELLTYPE_NOT);
        m_helper->triggerReRender();
        return;
    }
    if(action == "NEG")
    {
        m_smallTypes.insert(0, CELLTYPE_NEGATE);
        m_helper->triggerReRender();
        return;
    }
    if(action == "MINUS")
    {
        m_smallTypes.insert(0, CELLTYPE_MINUS);
        m_helper->triggerReRender();
        return;
    }
    if(action.startsWith("REMOVE"))
    {
        int idx = action.right(action.length() - 6).toInt();
        m_smallTypes.remove(idx);
        m_helper->triggerReRender();
        return;
    }
    if(action == "Function")
    {
        m_smallTypes.insert(0, CELLTYPE_FUNCTION);
        m_helper->triggerReRender();
        return;
    }

    if(action.startsWith("@"))
    {
        // this is a function...
    }
}

QVector<CellTypeChooserType> UnaryWhereExpressionQueryComponent::getChoosableTypes() const
{
    QVector<CellTypeChooserType> allowedTypes;
    allowedTypes.append(CELLTYPE_NOTHING);
    allowedTypes.append(CELLTYPE_NEGATE);
    allowedTypes.append(CELLTYPE_MINUS);
    allowedTypes.append(CELLTYPE_NOT);
    allowedTypes.append(CELLTYPE_FUNCTION);

    return allowedTypes;
}
