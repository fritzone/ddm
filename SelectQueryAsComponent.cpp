#include "SelectQueryAsComponent.h"

SelectQueryAsComponent::SelectQueryAsComponent(QueryComponent* p, int l):QueryComponent(p,l), m_as("A")
{
}


void SelectQueryAsComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
}

QSet<OptionsType> SelectQueryAsComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

QueryComponent* SelectQueryAsComponent::duplicate()
{
    SelectQueryAsComponent* newc = new SelectQueryAsComponent(m_parent, m_level);
    newc->m_as = m_as;
    return newc;
}
