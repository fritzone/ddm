#include "UnaryWhereExpressionQueryComponent.h"

UnaryWhereExpressionQueryComponent::UnaryWhereExpressionQueryComponent(QueryComponent* p, int l): WhereExpressionQueryComponent(p,l)
{
}

QueryGraphicsItem* UnaryWhereExpressionQueryComponent::createGraphicsItem(QueryGraphicsHelper *, QueryGraphicsItem *)
{
    return 0;
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
