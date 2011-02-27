#include "SelectQuerySelectComponent.h"

SelectQuerySelectComponent::SelectQuerySelectComponent(QueryComponent* p) : QueryComponent(p)
{
}

QSet<OptionsType> SelectQuerySelectComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}
