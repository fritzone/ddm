#include "qbr_SelectQueryFromComponent.h"
#include "strings.h"
#include "qbr_SelectQuery.h"
#include "Workspace.h"
#include "Version.h"
#include "Table.h"
#include "TableQueryComponent.h"

SelectQueryFromComponent::SelectQueryFromComponent(QueryComponent* p, int l):QueryComponent(p,l)
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

    t.insert(OPTIONS_NEW_SUBQUERY);

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
    SelectQueryFromComponent* newc = new SelectQueryFromComponent(m_parent, m_level);
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
