#include "qbr_SelectQueryGroupByComponent.h"
#include "qbr_SelectQuery.h"
#include "strings.h"

SelectQueryGroupByComponent::SelectQueryGroupByComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
}


void SelectQueryGroupByComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
    if(action == NEW_COLUMN)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            sq->newGroupByExpression();
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
    SelectQueryGroupByComponent *newc = new SelectQueryGroupByComponent(m_parent, m_level);
    return newc;
}
