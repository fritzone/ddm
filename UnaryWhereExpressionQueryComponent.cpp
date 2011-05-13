#include "UnaryWhereExpressionQueryComponent.h"
#include "CellForUnaryWhereExpression.h"

UnaryWhereExpressionQueryComponent::UnaryWhereExpressionQueryComponent(QueryComponent* p, int l): WhereExpressionQueryComponent(p,l)
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
