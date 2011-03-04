#include "SelectQuerySelectComponent.h"

SelectQuerySelectComponent::SelectQuerySelectComponent(QueryComponent* p, int l) : QueryComponent(p,l)
{
}

QSet<OptionsType> SelectQuerySelectComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

void SelectQuerySelectComponent::onClose()
{
    if(m_parent)
    {
        m_parent->onClose();
    }
}
