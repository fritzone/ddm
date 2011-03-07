#include "SelectQueryWhereComponent.h"

SelectQueryWhereComponent::SelectQueryWhereComponent(QueryComponent* p, int l, bool w):QueryComponent(p,l), m_trueWhere(w)
{
}


void SelectQueryWhereComponent::handleAction(const QString &action)
{
}

QSet<OptionsType> SelectQueryWhereComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}
