#include "SelectQuery.h"

SelectQuery::SelectQuery() : m_gitm(new SelectQueryGraphicsItem())
{
}

QueryGraphicsItem* SelectQuery::getGraphicsItem()
{
    return m_gitm;
}
