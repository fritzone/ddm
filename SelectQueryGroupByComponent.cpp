#include "SelectQueryGroupByComponent.h"

SelectQueryGroupByComponent::SelectQueryGroupByComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
}


void SelectQueryGroupByComponent::handleAction(const QString &action)
{
}

QSet<OptionsType> SelectQueryGroupByComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

