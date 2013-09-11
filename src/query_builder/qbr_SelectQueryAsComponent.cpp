#include "qbr_SelectQueryAsComponent.h"
#include "qbr_QueryAsGenerator.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "uids.h"

SelectQueryAsComponent::SelectQueryAsComponent(Query* q,
                                               QueryComponent* p,
                                               int l,
                                               Version *v) :
    QueryComponent(q, p, l, v)
{
    m_as = QueryAsGenerator::instance().getNextValidAs();
}


void SelectQueryAsComponent::handleAction(const QString &/*action*/,
                                          QueryComponent* /*referringObject*/)
{
}

QSet<OptionsType> SelectQueryAsComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

QueryComponent* SelectQueryAsComponent::duplicate()
{
    SelectQueryAsComponent* newc = new SelectQueryAsComponent(getQuery(),
                                                              getParent(),
                                                              getLevel(),
                                                              version());

    newc->m_as = (QueryAsGenerator::instance().getNextValidAs());
    return newc;
}

CloneableElement* SelectQueryAsComponent::clone(Version *sourceVersion,
                                                Version *targetVersion)
{
    SelectQueryAsComponent* n = dynamic_cast<SelectQueryAsComponent*>(duplicate());
    n->m_as = m_as;
    n->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, n);
    return n;
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
    if(getParent()->is<TableQueryComponent>())
    {
        getParent()->as<TableQueryComponent>()->removeAs();
    }

    if(getParent()->is<SingleExpressionQueryComponent>())
    {
        getParent()->as<SingleExpressionQueryComponent>()->removeAs();
    }

}

void SelectQueryAsComponent::serialize(QDomDocument& /*doc*/, QDomElement& /*parent*/) const
{
//    QDomElement asElement = doc.createElement("As");
//    QueryComponent::serialize(doc, asElement);
//    parent.appendChild(asElement);
}

QUuid SelectQueryAsComponent::getClassUid() const
{
    return QUuid(uidSelectQueryAsComponent);
}
