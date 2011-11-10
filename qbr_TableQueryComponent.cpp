#include "qbr_TableQueryComponent.h"
#include "Table.h"
#include "TableInstance.h"
#include "qbr_TableGraphicsItem.h"
#include "Workspace.h"
#include "Version.h"
#include "strings.h"
#include "qbr_SelectQueryAsComponent.h"
#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_CellAsCommand.h"
#include "qbr_CellJoinCommand.h"
#include "qbr_CellWhereCommand.h"
#include "Column.h"
#include "qbr_SingleExpressionQueryComponent.h"

#include <QGraphicsScene>
#include <QGraphicsView>

TableQueryComponent::TableQueryComponent(Table* tab, QueryComponent* p, int level):QueryComponent(p, level), m_table(tab), m_as(0), m_joins()
{
}

QueryGraphicsItem* TableQueryComponent::createGraphicsItem(QueryGraphicsHelper* helper, QueryGraphicsItem* parent)
{
    m_helper = helper;
    m_tgitm = new TableGraphicsItem(m_table, m_level, helper, parent, this);
    if(m_as)
    {
        m_tgitm->setAs(new CellAsCommand(helper, m_level + 1, m_tgitm, m_as, true));
    }
    for(int i=0; i<m_joins.size(); i++)
    {
        CellJoinCommand* joinCommand = new CellJoinCommand(helper, m_level + 1, m_tgitm, m_joins.at(i));
        CellWhereCommand* on = new CellWhereCommand(helper, m_level + 1, m_tgitm, m_joins.at(i), SelectQueryWhereComponent::WHERETYPE_ON);
        for(int j=0; j<m_joins.at(i)->getChildren().size(); j++)
        {
            QueryGraphicsItem* gritm = m_joins.at(i)->getChildren().at(j)->createGraphicsItem(helper, joinCommand);
            joinCommand->addChild(gritm);
        }
        for(int j=0; j<m_joins.at(i)->getJoinExpresions().size(); j++)
        {
            QueryGraphicsItem* gritm = m_joins.at(i)->getJoinExpresions().at(j)->createGraphicsItem(helper, on);
            dynamic_cast<SingleExpressionQueryComponent*> (m_joins.at(i)->getJoinExpresions().at(j))->setOwnedByOn(m_joins.at(i), m_joins.at(i)->getJoinExpresions().at(j));
            on->addChild(gritm);
        }
        m_tgitm->addJoin(joinCommand, on);
    }
    return m_tgitm;
}

void TableQueryComponent::setTable(const QString &tab)
{
    m_table = Workspace::getInstance()->workingVersion()->getTable(tab);
}

void TableQueryComponent::handleAction(const QString &action, QueryComponent *)
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
        SelectQueryJoinComponent* join = new SelectQueryJoinComponent(this, m_level + 1);
        join->setHelper(m_helper);
        //addChild(join);   // TODO: Check if this still works ...
        m_joins.append(join);
        m_helper->triggerReRender();

        return;
    }
    m_parent->handleAction(action, this);
}

QueryComponent* TableQueryComponent::duplicate()
{
    TableQueryComponent* newc = new TableQueryComponent(m_table, m_parent, m_level);
    newc->m_as = m_as?dynamic_cast<SelectQueryAsComponent*>(m_as->duplicate()):0;
    // Joins do not duplicate for now
    //for(int i=0; i<m_joins.size(); i++)
    //{
    //    SelectQueryJoinComponent* jI = dynamic_cast<SelectQueryJoinComponent*>(m_joins.at(i)->duplicate());
    //    newc->m_joins.append(jI);
    //}

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

void TableQueryComponent::removeJoin(SelectQueryJoinComponent* j)
{
    int idx = m_joins.indexOf(j);
    m_joins.remove(idx);
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

QString TableQueryComponent::get() const
{
    QString result = m_table->getName();
    if(!m_joins.isEmpty())
    {
        for(int i=0; i<m_joins.size(); i++)
        {
            result += "\n";
            result += getSpacesForLevel();
            result += m_joins.at(i)->get();
        }
    }
    if(m_as)
    {
        result += m_as->get();
    }

    return result;
}
void TableQueryComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableElement = doc.createElement("Table");
    tableElement.setAttribute("Name", m_table->getName());
    if(m_as)
    {
        tableElement.setAttribute("As", m_as->getAs());
    }
    if(!m_joins.empty())
    {
        QDomElement joinsElement = doc.createElement("Joins");
        for(int i=0; i< m_joins.size(); i++)
        {
            m_joins.at(i)->serialize(doc, joinsElement);
        }
        tableElement.appendChild(joinsElement);
    }
    QueryComponent::serialize(doc, tableElement);
    parent.appendChild(tableElement);
}
