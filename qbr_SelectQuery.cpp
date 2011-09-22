#include "qbr_SelectQuery.h"
#include "qbr_QueryComponents.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_SelectQueryFromComponent.h"
#include "qbr_SelectQuerySelectComponent.h"
#include "qbr_SelectQueryWhereComponent.h"
#include "qbr_SelectQueryAsComponent.h"
#include "qbr_SelectQueryGroupByComponent.h"
#include "qbr_SelectQueryOrderByComponent.h"
#include "strings.h"
#include "Workspace.h"
#include "Version.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_QueryAsGenerator.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>

SelectQuery::SelectQuery(QueryGraphicsHelper* helper, int level) : Query(helper, level), m_select(0),  m_from(0), m_where(0), m_groupby(0), m_having(0), m_as(0), m_orderBy(0)
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

    if(m_select)
    {
        for(int i=0; i<m_select->getChildren().size(); i++)
        {
            QueryGraphicsItem* gritmI = m_select->getChildren().at(i)->createGraphicsItem(m_helper, (QueryGraphicsItem*)gi->getSelect());
            gi->addSelectGraphicsItem(gritmI);
        }
    }

    if(m_from)
    {
        for(int i=0; i<m_from->getChildren().size(); i++)
        {
            QueryGraphicsItem* gritmI = m_from->getChildren().at(i)->createGraphicsItem(m_helper, (QueryGraphicsItem*)gi->getFrom());
            gi->addFromGraphicsItem(gritmI);
        }
    }

    if(m_where)
    {
        for(int i=0; i<m_where->getChildren().size(); i++)
        {
            QueryGraphicsItem* gritmI = m_where->getChildren().at(i)->createGraphicsItem(m_helper, (QueryGraphicsItem*)gi->getWhere());
            gi->addWhereGraphicsItem(gritmI);
        }
    }

    if(m_groupby)
    {
        for(int i=0; i<m_groupby->getChildren().size(); i++)
        {
            QueryGraphicsItem* gritmI = m_groupby->getChildren().at(i)->createGraphicsItem(m_helper, (QueryGraphicsItem*)gi->getGroupBy());
            gi->addGroupByGraphicsItem(gritmI);
        }
    }

    if(m_having)
    {
        for(int i=0; i<m_having->getChildren().size(); i++)
        {
            QueryGraphicsItem* gritmI = m_having->getChildren().at(i)->createGraphicsItem(m_helper, (QueryGraphicsItem*)gi->getHaving());
            gi->addHavingGraphicsItem(gritmI);
        }
    }

    if(m_orderBy)
    {
        for(int i=0; i<m_orderBy->getChildren().size(); i++)
        {
            QueryGraphicsItem* gritmI = m_orderBy->getChildren().at(i)->createGraphicsItem(m_helper, (QueryGraphicsItem*)gi->getOrderBy());
            gi->addOrderByGraphicsItem(gritmI);
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

void SelectQuery::newSelectExpression()
{
    if(m_select)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(m_select, m_level);
        m_select->addChild(c);
        m_helper->triggerReRender();
    }
}

void SelectQuery::newGroupByExpression()
{
    if(m_groupby)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(m_groupby, m_level);
        m_groupby->addChild(c);
        m_helper->triggerReRender();
    }
}

void SelectQuery::newOrderByExpression()
{
    if(m_orderBy)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(m_orderBy, m_level);
        m_orderBy->addChild(c);
        m_helper->triggerReRender();
    }
}

void SelectQuery::newWhereExpression(SelectQuery::NewWhereExpressionType t)
{
    if(m_where)
    {
        if(t == PLAIN_NEW_WHERE_EXPRESSION)
        {
            WhereExpressionQueryComponent* c = new SingleExpressionQueryComponent(m_where, m_level);
            m_where->addChild(c);
        }
        if(t == OR_NEW_WHERE_EXPRESSION)
        {
            WhereExpressionQueryComponent* orc = new SingleExpressionQueryComponent(m_where, m_level);
            dynamic_cast<SingleExpressionQueryComponent*>(orc)->setForcedType(SingleExpressionQueryComponent::FORCED_OR);
            m_where->addChild(orc);
        }
        if(t == AND_NEW_WHERE_EXPRESSION)
        {
            WhereExpressionQueryComponent* andc = new SingleExpressionQueryComponent(m_where, m_level);
            dynamic_cast<SingleExpressionQueryComponent*>(andc)->setForcedType(SingleExpressionQueryComponent::FORCED_AND);
            m_where->addChild(andc);
        }

        m_helper->triggerReRender();
    }
}

void SelectQuery::newHavingExpression(SelectQuery::NewWhereExpressionType t)
{
    if(m_having)
    {
        if(t == PLAIN_NEW_WHERE_EXPRESSION)
        {
            WhereExpressionQueryComponent* c = new SingleExpressionQueryComponent(m_having, m_level);
            m_having->addChild(c);
        }
        if(t == OR_NEW_WHERE_EXPRESSION)
        {
            WhereExpressionQueryComponent* orc = new SingleExpressionQueryComponent(m_having, m_level);
            dynamic_cast<SingleExpressionQueryComponent*>(orc)->setForcedType(SingleExpressionQueryComponent::FORCED_OR);
            m_having->addChild(orc);
        }
        if(t == AND_NEW_WHERE_EXPRESSION)
        {
            WhereExpressionQueryComponent* andc = new SingleExpressionQueryComponent(m_having, m_level);
            dynamic_cast<SingleExpressionQueryComponent*>(andc)->setForcedType(SingleExpressionQueryComponent::FORCED_AND);
            m_having->addChild(andc);
        }

        m_helper->triggerReRender();
    }
}

void SelectQuery::newFromTableComponent()
{
    if(m_from)
    {
        TableQueryComponent* tccp = TableQueryComponent::provideFirstTableIfAny(m_from, m_level);
        if(!tccp) return;
        m_from->addChild(tccp);
        m_helper->triggerReRender();
    }
}

void SelectQuery::duplicateFromsChild(QueryComponent *c)
{
    if(m_from)
    {
        QueryComponent* copy = c->duplicate();
        m_from->getChildren().insert(m_from->getChildren().indexOf(c), copy);
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


void SelectQuery::handleAction(const QString &action, QueryComponent* referringObject)
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
            m_where = new SelectQueryWhereComponent(this, m_level, SelectQueryWhereComponent::WHERETYPE_WHERE);
            m_helper->triggerReRender();
        }
    }
    if(action == ADD_GROUPBY)
    {
        if(!m_groupby)
        {
            m_groupby = new SelectQueryGroupByComponent(this, m_level);
            m_helper->triggerReRender();
        }
    }
    if(action == ADD_HAVING)
    {
        if(!m_having)
        {
            m_having = new SelectQueryWhereComponent(this, m_level, SelectQueryWhereComponent::WHERETYPE_HAVING);
            m_helper->triggerReRender();
        }
    }
    if(action == ADD_ORDERBY)
    {
        if(!m_orderBy)
        {
            m_orderBy = new SelectQueryOrderByComponent(this, m_level);
            m_helper->triggerReRender();
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
    return m_select->hasGroupByFunctions();
}

bool SelectQuery::hasAtLeastOneColumnSelected()
{
    return m_select->hasAtLeastOneColumnSelected();
}

QSet<OptionsType> SelectQuery::provideOptions()
{
    QSet<OptionsType> t;

    if(!m_from) t.insert(OPTIONS_ADD_FROM);
    if(!m_where && m_from) t.insert(OPTIONS_ADD_WHERE);
    if(m_from && hasGroupByFunctions() && hasAtLeastOneColumnSelected() && !m_groupby) t.insert(OPTIONS_ADD_GROUPBY);
    if(m_from && hasGroupByFunctions() && m_groupby && !m_having) t.insert(OPTIONS_ADD_HAVING);
    if(!m_orderBy) t.insert(OPTIONS_ADD_ORDERBY);

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

void SelectQuery::removeFrom()
{
    if(m_from)
    {
        delete m_from;
        m_from = 0;
        m_helper->triggerReRender();
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

QString SelectQuery::get() const
{
    QString result = "";
    if(m_select)
    {
        result += m_select->get();
        result += "\n";
    }
    if(m_from)
    {
        result += m_from->get();
        result += "\n";
    }
    if(m_where)
    {
        result += m_where->get();
        result += "\n";
    }
    if(m_groupby)
    {
        result += m_groupby->get();
        result += "\n";
    }
    if(m_having)
    {
        result += m_having->get();
        result += "\n";
    }
    if(m_orderBy)
    {
        result += m_orderBy->get();
        result += "\n";
    }
    if(m_as)
    {
        result += m_as->get();
        result += "\n";
    }

    return result;
}

QVector<const Table*> SelectQuery::getTables() const
{
    if(m_from) return m_from->getTables();
    else return QVector<const Table*> ();
}

QVector<const Column*> SelectQuery::getSelectedColumns()
{
    return m_select->getSelectedColumns();
}

QStringList SelectQuery::getOrderByElements()
{
    return m_select->getOrderByElements();
}
