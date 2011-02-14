#include "SelectQuery.h"

SelectQuery::SelectQuery(QueryComponents* comps) : m_gitm(new SelectQueryGraphicsItem(comps)), m_components(comps)
{
    m_gitm->test();
}


QueryGraphicsItem* SelectQuery::getGraphicsItem()
{
    return m_gitm;
}
