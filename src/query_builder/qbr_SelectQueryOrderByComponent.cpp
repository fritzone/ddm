#include "qbr_SelectQueryOrderByComponent.h"
#include "qbr_SelectQuery.h"
#include "uids.h"
#include "ddm_strings.h"

#include <QDebug>

SelectQueryOrderByComponent::SelectQueryOrderByComponent(Query* q, QueryComponent* p, int l, Version *v) :
    QueryComponent(q, p, l, v)
{
}


void SelectQueryOrderByComponent::handleAction(const QString &action, QueryComponent* /*referringObject*/)
{
    if(action == NEW_COLUMN)
    {
        if(getParent()->is<SelectQuery>())
        {
            getParent()->as<SelectQuery>()->newOrderByExpression();
        }
    }
}

QSet<OptionsType> SelectQueryOrderByComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_COLUMN);
    return t;
}

QueryComponent* SelectQueryOrderByComponent::duplicate()
{
    SelectQueryOrderByComponent* newc = new SelectQueryOrderByComponent(getQuery(), getParent(), getLevel(), version());
    return newc;
}

CloneableElement* SelectQueryOrderByComponent::clone(Version *sourceVersion, Version *targetVersion)
{
    SelectQueryOrderByComponent* newc = new SelectQueryOrderByComponent(getQuery(), getParent(), getLevel(), targetVersion);
    newc->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, newc);
    return newc;
}

QString SelectQueryOrderByComponent::get() const
{
    QString result = "ORDER BY";

    if(hasChildren())
    {
        result += strNewline;
    }

    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        result += getSpacesForLevel();
        result+= children[i]->get();
        if(i < children.size() - 1)
        {
            result += strComma;
        }
    }
    return result;
}

void SelectQueryOrderByComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement orderByElement = doc.createElement("OrderBy");
    QueryComponent::serialize(doc, orderByElement);
    parent.appendChild(orderByElement);
}

QUuid SelectQueryOrderByComponent::getClassUid() const
{
    return QUuid(uidSelectQueryOrderByComponent);
}
