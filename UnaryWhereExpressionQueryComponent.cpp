#include "UnaryWhereExpressionQueryComponent.h"
#include "CellForUnaryWhereExpression.h"

UnaryWhereExpressionQueryComponent::UnaryWhereExpressionQueryComponent(QueryComponent* p, int l): WhereExpressionQueryComponent(p,l)
{

    QSet<CellTypeChooserType> allowedTypes;
    allowedTypes.insert(CELLTYPE_NOTHING);
    allowedTypes.insert(CELLTYPE_NOT);
    allowedTypes.insert(CELLTYPE_NEGATE);

    m_smallTypes = allowedTypes;
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
        if(m_smallTypes.contains(CELLTYPE_NOT))
        {
            m_smallTypes.remove(CELLTYPE_NOT);
        }
        else
        {
            m_smallTypes.insert(CELLTYPE_NOT);
        }
    }
}
