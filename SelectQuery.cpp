#include "SelectQuery.h"
#include "QueryComponents.h"
#include "TableQueryComponent.h"
#include "SelectQueryFromComponent.h"
#include "SelectQuerySelectComponent.h"
#include "SelectQueryWhereComponent.h"
#include "TableInstance.h"
#include "strings.h"

#include "Workspace.h"
#include "Version.h"

SelectQuery::SelectQuery(QueryGraphicsHelper* helper, int level) : Query(helper, level), m_from(0), m_select(0), m_where(0)
{
    m_select = new SelectQuerySelectComponent(this, level);
}

QueryGraphicsItem* SelectQuery::createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*)
{
    SelectQueryGraphicsItem* gi = new SelectQueryGraphicsItem(m_helper, m_level, 0, this);
    if(m_from) gi->createFromCell(m_from);
    if(m_select) gi->createSelectCell(m_select);
    if(m_where) gi->createWhereCell(m_where);

    if(m_from)
    {
        for(int i=0; i<m_from->getChildren().size(); i++)
        {
            QueryGraphicsItem* gritmI = m_from->getChildren().at(i)->createGraphicsItem(m_helper, (QueryGraphicsItem*)gi->getFrom());
            gi->addFromGraphicsItem(gritmI);
        }
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
    if(m_from)
    {
        if(Workspace::getInstance()->currentProjectIsOop())
        {
            m_from->addChild(new TableQueryComponent(Workspace::getInstance()->workingVersion()->getTableInstances().at(0)->table(), m_from, m_level));
        }
        else
        {
            m_from->addChild(new TableQueryComponent(Workspace::getInstance()->workingVersion()->getTables().at(0), m_from, m_level));
        }

        m_helper->triggerReRender();
    }
}

void SelectQuery::newFromSelectQueryComponent()
{
    if(m_from)
    {
        SelectQuery* nq = new SelectQuery(m_helper, m_level + 1);
        m_from->addChild(nq);
        nq->setParent(m_from);
        m_helper->triggerReRender();
    }
}


void SelectQuery::handleAction(const QString &action)
{
    if(action == ADD_FROM)
    {
        if(!m_from)
        {
            m_from = new SelectQueryFromComponent(this, m_level);
            m_helper->triggerReRender();
        }
    }
    if(action == ADD_WHERE)
    {
        if(!m_where)
        {
            m_where = new SelectQueryWhereComponent(this, m_level);
            m_helper->triggerReRender();
        }
    }
}

QSet<OptionsType> SelectQuery::provideOptions()
{
    QSet<OptionsType> t;
    if(!m_from) t.insert(OPTIONS_ADD_FROM);
    if(!m_where && m_from) t.insert(OPTIONS_ADD_WHERE);
    return t;
}

void SelectQuery::onClose()
{
    if(m_parent)
    {
        m_parent->removeChild(this);
        m_helper->triggerReRender();
    }
}
