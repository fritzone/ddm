#include "SelectQueryFromComponent.h"
#include "strings.h"
#include "SelectQuery.h"

SelectQueryFromComponent::SelectQueryFromComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
}

void SelectQueryFromComponent::handleAction(const QString &action)
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
}

QSet<OptionsType> SelectQueryFromComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_TABLE);
    t.insert(OPTIONS_NEW_SUBQUERY);

    return t;
}
