#include "qbr_SelectQueryGroupByComponent.h"
#include "qbr_SelectQuery.h"
#include "strings.h"
#include "uids.h"

SelectQueryGroupByComponent::SelectQueryGroupByComponent(Query* q, QueryComponent* p, int l, Version *v) :
    QueryComponent(q, p, l, v)
{
}


void SelectQueryGroupByComponent::handleAction(const QString &action, QueryComponent* /*referringObject*/)
{
    if(action == NEW_COLUMN)
    {
        if(getParent()->is<SelectQuery>())
        {
            getParent()->as<SelectQuery>()->newGroupByExpression();
        }
    }
}

QSet<OptionsType> SelectQueryGroupByComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_COLUMN);
    return t;
}

QueryComponent* SelectQueryGroupByComponent::duplicate()
{
    SelectQueryGroupByComponent *newc = new SelectQueryGroupByComponent(getQuery(), getParent(), getLevel(), version());
    return newc;
}

CloneableElement* SelectQueryGroupByComponent::clone(Version *sourceVersion, Version *targetVersion)
{
    SelectQueryGroupByComponent *newc = new SelectQueryGroupByComponent(getQuery(), getParent(), getLevel(), targetVersion);
    newc->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, newc);
    return newc;
}

QString SelectQueryGroupByComponent::get() const
{
    QString result = "GROUP BY";
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
            result += strSpace;
        }
    }

    return result;
}

void SelectQueryGroupByComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement groupBy = doc.createElement("GroupBy");
    QueryComponent::serialize(doc, groupBy);
    parent.appendChild(groupBy);
}

QUuid SelectQueryGroupByComponent::getClassUid() const
{
    return QUuid(uidSelectQueryGroupByComponent);
}
