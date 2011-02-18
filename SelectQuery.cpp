#include "SelectQuery.h"
#include "QueryComponents.h"
#include "TableQueryComponent.h"
#include "SelectQueryFromComponent.h"
#include "SelectQuerySelectComponent.h"

#include "Workspace.h"
#include "Version.h"

SelectQuery::SelectQuery(QueryGraphicsHelper* helper) : Query(helper)
{
    m_from = new SelectQueryFromComponent(0);
}


bool SelectQuery::createGraphicsItem()
{
    SelectQueryGraphicsItem* gi = new SelectQueryGraphicsItem(m_helper, 0, 0, 0);
    for(int i=0; i<m_from->getChildren().size(); i++)
    {
        QueryGraphicsItem* gritmI = m_from->getChildren().at(i)->createGraphicsItem(m_helper, gi->getFrom());
        gi->addFromGraphicsItem(gritmI);
    }

    m_graphicsItem = gi;
    return true;
}


void SelectQuery::newFromComponent()
{
    m_from->addChild(new TableQueryComponent(Workspace::getInstance()->workingVersion()->getTables().at(0), m_from));
}

