#include "qbr_SelectQueryGroupByComponent.h"

SelectQueryGroupByComponent::SelectQueryGroupByComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
}


void SelectQueryGroupByComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
}

QSet<OptionsType> SelectQueryGroupByComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_COLUMN);
    return t;
}

QueryComponent* SelectQueryGroupByComponent::duplicate()
{
    SelectQueryGroupByComponent *newc = new SelectQueryGroupByComponent(m_parent, m_level);
    return newc;
}
