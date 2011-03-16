#include "SelectQueryWhereComponent.h"

SelectQueryWhereComponent::SelectQueryWhereComponent(QueryComponent* p, int l, bool w):QueryComponent(p,l), m_trueWhere(w)
{
}


void SelectQueryWhereComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
}

QSet<OptionsType> SelectQueryWhereComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

QueryComponent* SelectQueryWhereComponent::duplicate()
{
    SelectQueryWhereComponent* newc = new SelectQueryWhereComponent(m_parent, m_level, m_trueWhere);
    return newc;
}
