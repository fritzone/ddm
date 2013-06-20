#include "qbr_SelectQueryFromComponent.h"
#include "strings.h"
#include "qbr_SelectQuery.h"
#include "Workspace.h"
#include "Version.h"
#include "core_Table.h"
#include "qbr_TableQueryComponent.h"

SelectQueryFromComponent::SelectQueryFromComponent(QueryComponent* p, int l, Version *v):QueryComponent(p,l,v)
{
}

void SelectQueryFromComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
    if(action == NEW_TABLE)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            sq->newFromTableComponent();
        }
    }
    if(action == NEW_SUBQUERY)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            sq->newFromSelectQueryComponent();
        }
    }
    if(action == DUPLICATE)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
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
    if(m_parent)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            sq->removeFrom();
        }
    }
}

bool SelectQueryFromComponent::allowCloseButton()
{
    if(m_parent)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            return ! sq->hasWhere() && !sq->hasGroupBy();
        }
    }
    return false;
}

QueryComponent* SelectQueryFromComponent::duplicate()
{
    SelectQueryFromComponent* newc = new SelectQueryFromComponent(m_parent, m_level, version());
    return newc;
}

CloneableElement* SelectQueryFromComponent::clone(Version *sourceVersion, Version *targetVersion)
{
    SelectQueryFromComponent* newc = new SelectQueryFromComponent(m_parent, m_level, targetVersion);
    newc->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, newc);
    return newc;
}

QVector<const Table*> SelectQueryFromComponent::getTables() const
{
    QVector<const Table*>  result;
    for(int i=0; i<m_children.size(); i++)
    {
        TableQueryComponent* tc = dynamic_cast<TableQueryComponent*>(m_children.at(i));
        if(tc)
        {
            result.push_back(tc->getTable());
        }
    }
    return result;
}

QVector<const TableInstance*> SelectQueryFromComponent::getTableInstances() const
{
    QVector<const TableInstance*>  result;
    for(int i=0; i<m_children.size(); i++)
    {
        TableQueryComponent* tc = dynamic_cast<TableQueryComponent*>(m_children.at(i));
        if(tc)
        {
            result.push_back(tc->getTableInstance());
        }
    }
    return result;
}

QString SelectQueryFromComponent::get() const
{
    QString result = "FROM";
    if(m_children.size())
    {
        result += "\n";
    }

    for(int i=0; i<m_children.size(); i++)
    {
        result += getSpacesForLevel();
        result += m_children.at(i)->get();
        if(i<m_children.size() - 1)
        {
            result += ",\n";
        }
    }
    if(m_children.size()) result += " ";
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
