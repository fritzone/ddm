#include "SelectQueryFromComponent.h"
#include "strings.h"
#include "SelectQuery.h"

SelectQueryFromComponent::SelectQueryFromComponent(QueryComponent* p):QueryComponent(p)
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
}
