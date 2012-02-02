#include "qbr_SelectQueryAsComponent.h"
#include "qbr_QueryAsGenerator.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_SingleExpressionQueryComponent.h"

SelectQueryAsComponent::SelectQueryAsComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
    m_as = QueryAsGenerator::instance().getNextValidAs();
}


void SelectQueryAsComponent::handleAction(const QString &/*action*/, QueryComponent* /*referringObject*/)
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
    newc->m_as = (QueryAsGenerator::instance().getNextValidAs());
    return newc;
}

bool SelectQueryAsComponent::setAs(const QString& as)
{
    if(QueryAsGenerator::instance().hasAs(as)) return false;
    m_as = as;
    QueryAsGenerator::instance().addAs(as);
    return true;
}

void SelectQueryAsComponent::onClose()
{
    {
    TableQueryComponent* parent = dynamic_cast<TableQueryComponent*>(m_parent);
    if(parent)
    {
        parent->removeAs();
    }
    }

    {
    SingleExpressionQueryComponent* parent = dynamic_cast<SingleExpressionQueryComponent*>(m_parent);
    if(parent)
    {
        parent->removeAs();
    }
    }

}

void SelectQueryAsComponent::serialize(QDomDocument& /*doc*/, QDomElement& /*parent*/) const
{
//    QDomElement asElement = doc.createElement("As");
//    QueryComponent::serialize(doc, asElement);
//    parent.appendChild(asElement);
}