#include "qbr_SelectQueryFromComponent.h"
#include "strings.h"
#include "qbr_SelectQuery.h"
#include "Workspace.h"
#include "Version.h"
#include "core_Table.h"
#include "qbr_TableQueryComponent.h"

SelectQueryFromComponent::SelectQueryFromComponent(Query* q,
                                                   QueryComponent* p,
                                                   int l,
                                                   Version *v):
    QueryComponent(q, p, l, v)
{
}

void SelectQueryFromComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
    if(action == NEW_TABLE)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(getParent());
        if(sq)
        {
            sq->newFromTableComponent();
        }
    }
    if(action == NEW_SUBQUERY)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(getParent());
        if(sq)
        {
            sq->newFromSelectQueryComponent();
        }
    }
    if(action == DUPLICATE)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(getParent());
        if(sq)
        {
            sq->duplicateFromsChild(referringObject);
        }
    }
}

QSet<OptionsType> SelectQueryFromComponent::provideOptions()
{
    QSet<OptionsType> t;
    if(Workspace::getInstance()->currentProjectIsOop())
    {
        if(Workspace::getInstance()->workingVersion()->getTableInstances().size() > 0) t.insert(OPTIONS_NEW_TABLE);
    }
    else
    {
        if(Workspace::getInstance()->workingVersion()->getTables().size() > 0) t.insert(OPTIONS_NEW_TABLE);
    }

    // TODO: MySQL does not support subquerys in view creation. For now just disable it
    // and at later stages when we will support independent query creation find a way to
    // fix this
    // t.insert(OPTIONS_NEW_SUBQUERY);

    return t;
}

void SelectQueryFromComponent::onClose()
{
    if(hasParent())
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(getParent());
        if(sq)
        {
            sq->removeFrom();
        }
    }
}

bool SelectQueryFromComponent::allowCloseButton()
{
    if(hasParent())
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(getParent());
        if(sq)
        {
            return ! sq->hasWhere() && !sq->hasGroupBy();
        }
    }
    return false;
}

QueryComponent* SelectQueryFromComponent::duplicate()
{
    SelectQueryFromComponent* newc =
            new SelectQueryFromComponent(getQuery(), getParent(),
                                         getLevel(), version());

    return newc;
}

CloneableElement* SelectQueryFromComponent::clone(Version *sourceVersion, Version *targetVersion)
{
    SelectQueryFromComponent* newc =
            new SelectQueryFromComponent(getQuery(), getParent(),
                                         getLevel(), targetVersion);

    newc->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, newc);
    return newc;
}

QVector<const Table*> SelectQueryFromComponent::getTables() const
{
    QVector<const Table*> result;
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        if(children[i]->is<TableQueryComponent>())
        {
            result.push_back(children[i]->as<TableQueryComponent>()->getTable());
        }
    }
    return result;
}

QVector<const TableInstance*> SelectQueryFromComponent::getTableInstances() const
{
    QVector<const TableInstance*> result;
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        if(children[i]->is<TableQueryComponent>())
        {
            result.push_back(children[i]->as<TableQueryComponent>()->getTableInstance());
        }

    }
    return result;
}

QString SelectQueryFromComponent::get() const
{
    QString result = "FROM";
    if(hasChildren())
    {
        result += strNewline;
    }

    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        result += getSpacesForLevel();
        result += children[i]->get();
        if(i<children.size() - 1)
        {
            result += strComma + strNewline;
        }
    }
    if(hasChildren())
    {
        result += " ";
    }
    return result;
}

void SelectQueryFromComponent::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement fromElement = doc.createElement("From");
    QueryComponent::serialize(doc, fromElement);
    parent.appendChild(fromElement);
}

QUuid SelectQueryFromComponent::getClassUid() const
{
    return QUuid(uidSelectQueryFromComponent);
}
