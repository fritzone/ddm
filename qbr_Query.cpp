#include "qbr_Query.h"

Query::Query(QueryGraphicsHelper* components, int level): QueryComponent(this, level), m_helper(components), m_graphicsItem(0)
{
}
