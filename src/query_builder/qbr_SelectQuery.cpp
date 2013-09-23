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
#include "uids.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>
#include <QDebug>

SelectQuery::SelectQuery(QueryGraphicsHelper* helper, int level, SqlSource* se, Version *v) :
    Query(helper, level, se, v),
    m_select(0),
    m_from(0),
    m_where(0),
    m_groupby(0),
    m_having(0),
    m_as(0),
    m_orderBy(0)
{
    m_select = new SelectQuerySelectComponent(this, this, level, v);

    if(getLevel() > 0)
    {
        m_as = new SelectQueryAsComponent(this, this, level, v);
    }
}

SelectQuery::SelectQuery(QueryGraphicsHelper* helper, SqlSource* se, Version *v) :
    Query(helper, 0, se, v),
    m_select(0),
    m_from(0),
    m_where(0),
    m_groupby(0),
    m_having(0),
    m_as(0),
    m_orderBy(0)
{
}

QueryGraphicsItem* SelectQuery::createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*)
{
    SelectQueryGraphicsItem* gi = new SelectQueryGraphicsItem(
                m_helper, getLevel(), 0, this);

    if(m_from)
    {
        gi->createFromCell(m_from);
    }

    if(m_select)
    {
        gi->createSelectCell(m_select);
    }

    if(m_where)
    {
        gi->createWhereCell(m_where);
    }

    if(m_groupby)
    {
        gi->createGroupByCell(m_groupby);
    }

    if(m_having)
    {
        gi->createHavingCell(m_having);
    }

    if(m_as)
    {
        gi->createAsCell(m_as);
    }

    if(m_orderBy)
    {
        gi->createOrderByCell(m_orderBy);
    }

    if(m_select)
    {
        const QList<QueryComponent*>& selectsChildren = m_select->getChildren();
        for(int i=0; i<selectsChildren.size(); i++)
        {
            QueryGraphicsItem* gritmI = selectsChildren[i]->createGraphicsItem(m_helper,
                                                                               (QueryGraphicsItem*)gi->getSelect());
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
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(getQuery(), m_select, getLevel(), version());
        m_select->addChild(c);
        m_helper->triggerReRender();
    }
}

void SelectQuery::newGroupByExpression()
{
    if(m_groupby)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(getQuery(), m_groupby, getLevel(), version());
        m_groupby->addChild(c);
        m_helper->triggerReRender();
    }
}

void SelectQuery::newOrderByExpression()
{
    if(m_orderBy)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(getQuery(), m_orderBy, getLevel(), version());
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
            SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(getQuery(), m_where, getLevel(), version());
            m_where->addChild(c);
        }
        if(t == OR_NEW_WHERE_EXPRESSION)
        {
            SingleExpressionQueryComponent* orc = new SingleExpressionQueryComponent(getQuery(), m_where, getLevel(), version());
            dynamic_cast<SingleExpressionQueryComponent*>(orc)->setForcedType(SingleExpressionQueryComponent::FORCED_OR);
            m_where->addChild(orc);
        }
        if(t == AND_NEW_WHERE_EXPRESSION)
        {
            SingleExpressionQueryComponent* andc = new SingleExpressionQueryComponent(getQuery(), m_where, getLevel(), version());
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
            SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(getQuery(), m_having, getLevel(), version());
            m_having->addChild(c);
        }
        if(t == OR_NEW_WHERE_EXPRESSION)
        {
            SingleExpressionQueryComponent* orc = new SingleExpressionQueryComponent(getQuery(), m_having, getLevel(), version());
            dynamic_cast<SingleExpressionQueryComponent*>(orc)->setForcedType(SingleExpressionQueryComponent::FORCED_OR);
            m_having->addChild(orc);
        }
        if(t == AND_NEW_WHERE_EXPRESSION)
        {
            SingleExpressionQueryComponent* andc = new SingleExpressionQueryComponent(getQuery(), m_having, getLevel(), version());
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
        TableQueryComponent* tccp = TableQueryComponent::provideFirstTableIfAny(m_from, getLevel());
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
        m_from->insertChild(m_from->getChildren().indexOf(c), copy);
        m_helper->triggerReRender();
    }
}

void SelectQuery::newFromSelectQueryComponent()
{
    if(m_from)
    {
        SelectQuery* nq = new SelectQuery(m_helper, nextLevel(), m_sqlSource, version());
        m_from->addChild(nq);
        nq->setParent(m_from);
        m_helper->triggerReRender();
    }
}


void SelectQuery::handleAction(const QString &action, QueryComponent* /*referringObject*/)
{
    if(action == ADD_FROM)
    {
        if(!m_from)
        {
            m_from = new SelectQueryFromComponent(getQuery(), this, getLevel(), version());
            m_helper->triggerReRender();
        }
    }
    if(action == ADD_WHERE)
    {
        if(!m_where)
        {
            m_where = new SelectQueryWhereComponent(getQuery(), this, getLevel(), SelectQueryWhereComponent::WHERETYPE_WHERE, version());
            m_helper->triggerReRender();
        }
    }
    if(action == ADD_GROUPBY)
    {
        if(!m_groupby)
        {
            m_groupby = new SelectQueryGroupByComponent(getQuery(), this, getLevel(), version());
            m_helper->triggerReRender();
        }
    }
    if(action == ADD_HAVING)
    {
        if(!m_having)
        {
            m_having = new SelectQueryWhereComponent(getQuery(), this, getLevel(), SelectQueryWhereComponent::WHERETYPE_HAVING, version());
            m_helper->triggerReRender();
        }
    }
    if(action == ADD_ORDERBY)
    {
        if(!m_orderBy)
        {
            m_orderBy = new SelectQueryOrderByComponent(getQuery(), this, getLevel(), version());
            m_helper->triggerReRender();
        }
    }
    if(action == STR_DUPLICATE)
    {
        if(getParent())
        {
            getParent()->handleAction(STR_DUPLICATE, this);
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
    if(getParent())
    {
        getParent()->removeChild(this);
        m_helper->triggerReRender();
    }
}

bool SelectQuery::hasFrom()
{
    return m_from != 0;
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
    SelectQuery* newQuery = new SelectQuery(m_helper, getLevel(), m_sqlSource, version());
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

CloneableElement* SelectQuery::clone(Version *sourceVersion, Version *targetVersion)
{
    SelectQuery* newQuery = new SelectQuery(m_helper, getLevel(), m_sqlSource, targetVersion);
    // TODO: set the parent from the new version: newQuery->setParent(getParent()); when there will be support for multiple queries.
    newQuery->m_select = m_select?dynamic_cast<SelectQuerySelectComponent*> (m_select->clone(sourceVersion, targetVersion)):0;
    newQuery->m_from = m_from?dynamic_cast<SelectQueryFromComponent*>(m_from->clone(sourceVersion, targetVersion)):0;
    newQuery->m_where = m_where?dynamic_cast<SelectQueryWhereComponent*>(m_where->clone(sourceVersion, targetVersion)):0;
    newQuery->m_groupby = m_groupby?dynamic_cast<SelectQueryGroupByComponent*>(m_groupby->clone(sourceVersion, targetVersion)):0;
    newQuery->m_having = m_having?dynamic_cast<SelectQueryWhereComponent*>(m_having->clone(sourceVersion, targetVersion)):0;
    newQuery->m_as = m_as?dynamic_cast<SelectQueryAsComponent*>(m_as->clone(sourceVersion, targetVersion)):0;
    newQuery->m_orderBy = m_orderBy?dynamic_cast<SelectQueryOrderByComponent*>(m_orderBy->clone(sourceVersion, targetVersion)):0;

    newQuery->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, newQuery);
    return newQuery;
}

QString SelectQuery::get() const
{
    QString result = "";
    if(m_select)
    {
        result += m_select->get();
        result += strNewline;
    }
    if(m_from)
    {
        result += m_from->get();
        result += strNewline;
    }
    if(m_where)
    {
        result += m_where->get();
    }
    if(m_groupby)
    {
        result += m_groupby->get();
        result += strNewline;
    }
    if(m_having)
    {
        result += m_having->get();
        result += strNewline;
    }
    if(m_orderBy)
    {
        result += m_orderBy->get();
        result += strNewline;
    }
    if(m_as)
    {
        result += m_as->get();
        result += strNewline;
    }

    return result;
}

QVector<const Table*> SelectQuery::getTables() const
{
    if(m_from) return m_from->getTables();
    else return QVector<const Table*> ();
}

QVector<const TableInstance*> SelectQuery::getTableInstances() const
{
    if(m_from) return m_from->getTableInstances();
    else return QVector<const TableInstance*> ();
}

QVector<const ColumnOfTabWithTabInstance*> SelectQuery::getSelectedColumns()
{
    return m_select->getSelectedColumns();
}

QStringList SelectQuery::getOrderByElements()
{
    return m_select->getOrderByElements();
}

QVector<const QueryComponent*> SelectQuery::getSelectedComponents()
{
    return m_select->getSelectedComponents();
}


void SelectQuery::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement queryElement = doc.createElement("Query");
    queryElement.setAttribute("Type", "Select");
    queryElement.setAttribute("uid", getObjectUid().toString());
    queryElement.setAttribute("class-uid", getClassUid().toString());

    if(m_select)
    {
        queryElement.setAttribute("HasSelect", 1);
        m_select->serialize(doc, queryElement);
    }
    else
    {
        queryElement.setAttribute("HasSelect", 0);
    }

    if(m_from)
    {
        queryElement.setAttribute("HasFrom", 1);
        m_from->serialize(doc, queryElement);
    }
    else
    {
        queryElement.setAttribute("HasFrom", 0);
    }

    if(m_where)
    {
        queryElement.setAttribute("HasWhere", 1);
        m_where->serialize(doc, queryElement);
    }
    else
    {
        queryElement.setAttribute("HasWhere", 0);
    }

    if(m_groupby)
    {
        queryElement.setAttribute("HasGroupBy", 1);
        m_groupby->serialize(doc, queryElement);
    }
    else
    {
        queryElement.setAttribute("HasGroupBy", 0);
    }

    if(m_having)
    {
        queryElement.setAttribute("HasHaving", 1);
        m_having->serialize(doc, queryElement);
    }
    else
    {
        queryElement.setAttribute("HasHaving", 0);
    }

    if(m_orderBy)
    {
        queryElement.setAttribute("HasOrderBy", 1);
        m_orderBy->serialize(doc, queryElement);
    }
    else
    {
        queryElement.setAttribute("HasOrderBy", 0);
    }

    if(m_as)
    {
        queryElement.setAttribute("HasAs", 1);
        m_as->serialize(doc, queryElement);
    }
    else
    {
        queryElement.setAttribute("HasAs", 0);
    }

    parent.appendChild(queryElement);

}

QUuid SelectQuery::getClassUid() const
{
    return QUuid(uidSelectQuery);
}

void SelectQuery::tableRemovedFromQuery(const QString &/*tabName*/)
{
    // TODO: Implement this, by walking round the children and notifying them
    // about the event.
}
