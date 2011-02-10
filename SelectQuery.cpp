#include "SelectQuery.h"

SelectQuery::SelectQuery() : m_gitm(new SelectQueryGraphicsItem())
{
    m_gitm->test();
}


QueryGraphicsItem* SelectQuery::getGraphicsItem()
{
    return m_gitm;
}
