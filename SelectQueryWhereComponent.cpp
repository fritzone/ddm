#include "SelectQueryWhereComponent.h"

SelectQueryWhereComponent::SelectQueryWhereComponent(QueryComponent* p, int l):QueryComponent(p,l)
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
