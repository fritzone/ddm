#include "SelectQuery.h"
#include "QueryComponents.h"
#include "TableQueryComponent.h"
#include "SelectQueryFromComponent.h"
#include "SelectQuerySelectComponent.h"
#include "SelectQueryWhereComponent.h"

#include "Workspace.h"
#include "Version.h"

SelectQuery::SelectQuery(QueryGraphicsHelper* helper) : Query(helper), m_from(0), m_select(0), m_where(0)
{
    m_from = new SelectQueryFromComponent(this);
    m_select = new SelectQuerySelectComponent(this);
}


QueryGraphicsItem* SelectQuery::createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*)
{
    SelectQueryGraphicsItem* gi = new SelectQueryGraphicsItem(m_helper, 0, 0, this);
    if(m_from) gi->createFromCell(m_from);
    if(m_select) gi->createSelectCell(m_select);
    if(m_where) gi->createWhereCell(m_where);

    for(int i=0; i<m_from->getChildren().size(); i++)
    {
        QueryGraphicsItem* gritmI = m_from->getChildren().at(i)->createGraphicsItem(m_helper, (QueryGraphicsItem*)gi->getFrom());
        gi->addFromGraphicsItem(gritmI);
    }

    m_graphicsItem = gi;
    return gi;
}

bool SelectQuery::initializeGraphicsItem()
{
    createGraphicsItem(0,0);
    return true;
}


void SelectQuery::newFromTableComponent()
{
    m_from->addChild(new TableQueryComponent(Workspace::getInstance()->workingVersion()->getTables().at(0), m_from));
    m_helper->triggerReRender();
}
