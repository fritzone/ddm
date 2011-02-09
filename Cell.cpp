#include "Cell.h"


Cell::Cell(): m_children()
{
}

void Cell::addChild(QueryGraphicsItem *itm)
{
    m_children.append(itm);
}
