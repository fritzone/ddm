#include "QueryComponents.h"

void QueryComponent::removeChild(QueryComponent *c)
{
    m_children.removeOne(c);
}
