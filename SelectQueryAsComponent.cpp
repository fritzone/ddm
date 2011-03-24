#include "SelectQueryAsComponent.h"
#include "QueryAsGenerator.h"

SelectQueryAsComponent::SelectQueryAsComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
    m_as = QueryAsGenerator::instance().getNextValidAs();
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

bool SelectQueryAsComponent::setAs(const QString& as)
{
    if(QueryAsGenerator::instance().hasAs(as)) return false;
    m_as = as;
    QueryAsGenerator::instance().addAs(as);
    return true;
}
