#include "TableQueryComponent.h"
#include "Table.h"
#include "TableInstance.h"
#include "TableGraphicsItem.h"
#include "Workspace.h"
#include "Version.h"
#include "strings.h"
#include "qbr_SelectQueryAsComponent.h"
#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_CellAsCommand.h"
#include "qbr_CellJoinCommand.h"
#include "qbr_CellWhereCommand.h"
#include "Column.h"

#include <QGraphicsScene>
#include <QGraphicsView>

TableQueryComponent::TableQueryComponent(Table* tab, QueryComponent* p, int level):QueryComponent(p, level), m_table(tab), m_as(0), m_join(0)
{
}

QString TableQueryComponent::get()
{
    return m_table->getName();
}

QueryGraphicsItem* TableQueryComponent::createGraphicsItem(QueryGraphicsHelper* helper, QueryGraphicsItem* parent)
{
    m_helper = helper;
    m_tgitm = new TableGraphicsItem(m_table, m_level, helper, parent, this);
    if(m_as)
    {
        m_tgitm->setAs(new CellAsCommand(helper, m_level + 1, m_tgitm, m_as, true));
    }
    if(m_join)
    {
        CellJoinCommand* joinCommand = new CellJoinCommand(helper, m_level + 1, m_tgitm, m_join);
        CellWhereCommand* on = new CellWhereCommand(helper, m_level + 1, m_tgitm, m_join, SelectQueryWhereComponent::WHERETYPE_ON);
        for(int i=0; i<m_join->getChildren().size(); i++)
        {
            QueryGraphicsItem* gritm = m_join->getChildren().at(i)->createGraphicsItem(helper, joinCommand);
            joinCommand->addChild(gritm);
        }
        m_tgitm->setJoin(joinCommand, on);
    }
    return m_tgitm;
}

void TableQueryComponent::setTable(const QString &tab)
{
    m_table = Workspace::getInstance()->workingVersion()->getTable(tab);
}

void TableQueryComponent::handleAction(const QString &action, QueryComponent *referringObject)
{
    if(action == ADD_ALIAS)
    {
        if(m_as == 0)
        {
            m_as = new SelectQueryAsComponent(this, m_level + 1);
            addChild(m_as);
            m_helper->triggerReRender();
        }
        return;
    }
    if(action == ADD_JOIN)
    {
        if(m_join == 0)
        {
            m_join = new SelectQueryJoinComponent(this, m_level + 1);
            m_join->setHelper(m_helper);
            addChild(m_join);
            m_helper->triggerReRender();
        }
        return;
    }
    m_parent->handleAction(action, this);
}

QueryComponent* TableQueryComponent::duplicate()
{
    TableQueryComponent* newc = new TableQueryComponent(m_table, m_parent, m_level);
    newc->m_as = m_as?dynamic_cast<SelectQueryAsComponent*>(m_as->duplicate()):0;

    return newc;
}

QSet<OptionsType> TableQueryComponent::provideOptions()
{
    QSet<OptionsType> result;
    result.insert(OPTIONS_ADD_JOIN);
    result.insert(OPTIONS_AS);
    return result;
}

void TableQueryComponent::removeAs()
{
    m_as = 0;
    m_helper->triggerReRender();
}

void TableQueryComponent::removeJoin()
{
    m_join = 0;
    m_helper->triggerReRender();
}

TableQueryComponent* TableQueryComponent::provideFirstTableIfAny(QueryComponent* parent, int level)
{
    TableQueryComponent* tccp = 0;
    if(Workspace::getInstance()->currentProjectIsOop())
    {
        if(Workspace::getInstance()->workingVersion()->getTableInstances().size() > 0)
        {
            tccp = new TableQueryComponent(Workspace::getInstance()->workingVersion()->getTableInstances().at(0)->table(), parent, level);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if(Workspace::getInstance()->workingVersion()->getTables().size() > 0)
        {
            tccp = new TableQueryComponent(Workspace::getInstance()->workingVersion()->getTables().at(0), parent, level);
        }
        else
        {
            return 0;
        }
    }
    return tccp;
}

QVector<const Column*> TableQueryComponent::provideColumns()
{
    QStringList cs = m_table->fullColumns();
    QVector<const Column*> result;

    for(int i=0; i<cs.size(); i++)
    {
        const Column* c = m_table->getColumn(cs.at(i));
        if(c == 0)
        {
            c = m_table->getColumnFromParents(cs.at(i));
            if(c == 0)
            {
                return QVector<const Column*>();
            }
        }
        result.push_back(c);
    }
    return result;
}
