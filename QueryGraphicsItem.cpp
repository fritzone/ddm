#include "QueryGraphicsItem.h"

QueryGraphicsItem::QueryGraphicsItem(QueryGraphicsItem* parent, QueryGraphicsHelper* comps, QueryComponent* owner) : m_children(), m_parent(parent), m_helper(comps), m_owner(owner)
{
}

void QueryGraphicsItem::addChild(QueryGraphicsItem *itm)
{
    m_children.append(itm);
}

void QueryGraphicsItem::removeChild(QueryGraphicsItem *child)
{
    m_children.removeOne(child);
}
