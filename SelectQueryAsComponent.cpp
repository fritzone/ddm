#include "SelectQueryAsComponent.h"

SelectQueryAsComponent::SelectQueryAsComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
}


void SelectQueryAsComponent::handleAction(const QString &action)
{
}

QSet<OptionsType> SelectQueryAsComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}
