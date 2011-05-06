#include "BinaryWhereExpressionQueryComponent.h"
#include "UnaryWhereExpressionQueryComponent.h"
#include "CellForBinaryWhereExpression.h"

BinaryWhereExpressionQueryComponent::BinaryWhereExpressionQueryComponent(QueryComponent* p, int l): WhereExpressionQueryComponent(p,l),
    m_left(0), m_right(0)
{
}

QueryGraphicsItem* BinaryWhereExpressionQueryComponent::createGraphicsItem(QueryGraphicsHelper *helper, QueryGraphicsItem *parent)
{
    m_helper = helper;
    m_gritm = new CellForBinaryWhereExpression(getLevel() + 1, helper, parent, this);

    return m_gritm;
}

QString BinaryWhereExpressionQueryComponent::get()
{
    return "Implement this";
}

QSet<OptionsType> BinaryWhereExpressionQueryComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

QueryComponent* BinaryWhereExpressionQueryComponent::duplicate()
{
    BinaryWhereExpressionQueryComponent* newc = new BinaryWhereExpressionQueryComponent(m_parent, m_level);
    return newc;
}

void BinaryWhereExpressionQueryComponent::createSides()
{
    m_left = new UnaryWhereExpressionQueryComponent(m_parent, m_level);
    m_right = new UnaryWhereExpressionQueryComponent(m_parent, m_level);
}

