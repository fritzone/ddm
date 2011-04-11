#include "SelectQuery.h"
#include "QueryComponents.h"
#include "TableQueryComponent.h"
#include "SelectQueryFromComponent.h"
#include "SelectQuerySelectComponent.h"
#include "SelectQueryWhereComponent.h"
#include "SelectQueryAsComponent.h"
#include "SelectQueryGroupByComponent.h"
#include "SelectQueryOrderByComponent.h"
#include "TableInstance.h"
#include "strings.h"
#include "Workspace.h"
#include "Version.h"
#include "QueryAsGenerator.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>

SelectQuery::SelectQuery(QueryGraphicsHelper* helper, int level) : Query(helper, level), m_from(0), m_select(0), m_where(0), m_groupby(0), m_having(0), m_as(0), m_orderBy(0)
{
    m_select = new SelectQuerySelectComponent(this, level);
    if(m_level > 0) m_as = new SelectQueryAsComponent(this, level);
}

QueryGraphicsItem* SelectQuery::createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*)
{
    SelectQueryGraphicsItem* gi = new SelectQueryGraphicsItem(m_helper, m_level, 0, this);
    if(m_from) gi->createFromCell(m_from);
    if(m_select) gi->createSelectCell(m_select);
    if(m_where) gi->createWhereCell(m_where);
    if(m_groupby) gi->createGroupByCell(m_groupby);
    if(m_having) gi->createHavingCell(m_having);
    if(m_as) gi->createAsCell(m_as);
    if(m_orderBy) gi->createOrderByCell(m_orderBy);

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


        QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
        int h = centerb.x();
        int v = centerb.y();

        m_helper->triggerReRender(h, v);
    }
}

void SelectQuery::duplicateFromsChild(QueryComponent *c)
{
    if(m_from)
    {
        QueryComponent* copy = c->duplicate();
        m_from->getChildren().insert(m_from->getChildren().indexOf(c), copy);

        QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
        int h = centerb.x();
        int v = centerb.y();

        m_helper->triggerReRender(h, v);
    }
    //m_children.insert(m_children.indexOf(c), c);
}

void SelectQuery::newFromSelectQueryComponent()
{
    if(m_from)
    {
        SelectQuery* nq = new SelectQuery(m_helper, m_level + 1);
        m_from->addChild(nq);
        nq->setParent(m_from);
        QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
        int h = centerb.x();
        int v = centerb.y();

        m_helper->triggerReRender(h, v);
    }
}


void SelectQuery::handleAction(const QString &action, QueryComponent* referringObject)
{
    if(action == ADD_FROM)
    {
        if(!m_from)
        {
            m_from = new SelectQueryFromComponent(this, m_level);
            QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
            int h = centerb.x();
            int v = centerb.y();

            m_helper->triggerReRender(h,v);
        }
    }
    if(action == ADD_WHERE)
    {
        if(!m_where)
        {
            m_where = new SelectQueryWhereComponent(this, m_level, true);
            QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
            int h = centerb.x();
            int v = centerb.y();

            m_helper->triggerReRender(h,v);
        }
    }
    if(action == ADD_GROUPBY)
    {
        if(!m_groupby)
        {
            m_groupby = new SelectQueryGroupByComponent(this, m_level);
            QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
            int h = centerb.x();
            int v = centerb.y();

            m_helper->triggerReRender(h,v);
        }
    }
    if(action == ADD_HAVING)
    {
        if(!m_having)
        {
            m_having = new SelectQueryWhereComponent(this, m_level, false);
            QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
            int h = centerb.x();
            int v = centerb.y();

            m_helper->triggerReRender(h,v);
        }
    }
    if(action == ADD_ORDERBY)
    {
        if(!m_orderBy)
        {
            m_orderBy = new SelectQueryOrderByComponent(this, m_level);

            QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
            int h = centerb.x();
            int v = centerb.y();

            m_helper->triggerReRender(h,v);
        }
    }
    if(action==DUPLICATE)
    {
        if(m_parent)
        {
            m_parent->handleAction(DUPLICATE, this);
        }
    }

}

bool SelectQuery::hasGroupByFunctions()
{
    return true;
}

QSet<OptionsType> SelectQuery::provideOptions()
{
    QSet<OptionsType> t;

    if(!m_from) t.insert(OPTIONS_ADD_FROM);
    if(!m_where && m_from) t.insert(OPTIONS_ADD_WHERE);
    if(m_from && hasGroupByFunctions() && !m_groupby) t.insert(OPTIONS_ADD_GROUPBY);
    if(m_from && hasGroupByFunctions() && m_groupby && !m_having) t.insert(OPTIONS_ADD_HAVING);
    if(!m_orderBy) t.insert(OPTIONS_ADD_ORDERBY);

    return t;
}

void SelectQuery::onClose()
{
    if(m_parent)
    {
        QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
        int h = centerb.x();
        int v = centerb.y();

        m_parent->removeChild(this);
        m_helper->triggerReRender(h, v);
    }
}

void SelectQuery::removeFrom()
{
    if(m_from)
    {
        QPointF centerb = m_graphicsItem->scene()->views().at(0)->mapToScene(m_graphicsItem->scene()->views().at(0)->viewport()->rect().center());
        int h = centerb.x();
        int v = centerb.y();

        delete m_from;
        m_from = 0;
        m_helper->triggerReRender(h, v);
    }
}

bool SelectQuery::hasWhere()
{
    return ! (m_where == 0);
}

bool SelectQuery::hasGroupBy()
{
    return ! (m_groupby == 0);
}

QueryComponent* SelectQuery::duplicate()
{
    SelectQuery* newQuery = new SelectQuery(m_helper, m_level);
    newQuery->setParent(getParent());
    newQuery->m_select = m_select?dynamic_cast<SelectQuerySelectComponent*> (m_select->duplicate()):0;
    newQuery->m_from = m_from?dynamic_cast<SelectQueryFromComponent*>(m_from->duplicate()):0;
    newQuery->m_where = m_where?dynamic_cast<SelectQueryWhereComponent*>(m_where->duplicate()):0;
    newQuery->m_groupby = m_groupby?dynamic_cast<SelectQueryGroupByComponent*>(m_groupby->duplicate()):0;
    newQuery->m_having = m_having?dynamic_cast<SelectQueryWhereComponent*>(m_having->duplicate()):0;
    newQuery->m_as = m_as?dynamic_cast<SelectQueryAsComponent*>(m_as->duplicate()):0;
    newQuery->m_orderBy = m_orderBy?dynamic_cast<SelectQueryOrderByComponent*>(m_orderBy->duplicate()):0;
    return newQuery;
}
