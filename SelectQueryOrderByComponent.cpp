#include "SelectQueryOrderByComponent.h"

SelectQueryOrderByComponent::SelectQueryOrderByComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
}


void SelectQueryOrderByComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
}

QSet<OptionsType> SelectQueryOrderByComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

QueryComponent* SelectQueryOrderByComponent::duplicate()
{
    SelectQueryOrderByComponent* newc = new SelectQueryOrderByComponent(m_parent, m_level);
    return newc;
}
