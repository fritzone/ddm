#include "qbr_TableQueryComponent.h"

#include "core_Table.h"
#include "core_TableInstance.h"
#include "core_Column.h"
#include "Workspace.h"
#include "Version.h"

#include "qbr_SelectQueryAsComponent.h"
#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_CellAsCommand.h"
#include "qbr_CellJoinCommand.h"
#include "qbr_CellWhereCommand.h"
#include "qbr_Query.h"
#include "qbr_TableGraphicsItem.h"
#include "qbr_SingleExpressionQueryComponent.h"

#include "UidWarehouse.h"
#include "ddm_strings.h"

#include <QGraphicsScene>
#include <QGraphicsView>

TableQueryComponent::TableQueryComponent(Query* q, Table* tab, QueryComponent* p,
                                         int level, Version *v):
    QueryComponent(q, p, level, v),
    m_table(tab), m_tinst(0), m_tabInsteadOfTinst(true), m_as(0), m_joins()
{
}

TableQueryComponent::TableQueryComponent(Query* q, TableInstance* tinst,
                                         QueryComponent* p, int level, Version *v):
    QueryComponent(q, p, level, v),
    m_table(0), m_tinst(tinst), m_tabInsteadOfTinst(false), m_as(0), m_joins()
{
}

QueryGraphicsItem* TableQueryComponent::createGraphicsItem(QueryGraphicsHelper* helper, QueryGraphicsItem* parent)
{
    m_helper = helper;
    m_tgitm = m_tabInsteadOfTinst?new TableGraphicsItem(m_table->getName(), getLevel(), helper, parent, this):
                                  new TableGraphicsItem(m_tinst->getName(), getLevel(), helper, parent, this);;
    if(m_as)
    {
        m_tgitm->setAs(new CellAsCommand(helper, nextLevel(), m_tgitm, m_as, true));
        getQuery()->setQueryGlobalAlias(m_as, this);
    }
    for(int i=0; i<m_joins.size(); i++)
    {
        m_joins.at(i)->setHelper(m_helper);
        CellJoinCommand* joinCommand = new CellJoinCommand(helper, nextLevel(), m_tgitm, m_joins.at(i));
        CellWhereCommand* on = new CellWhereCommand(helper, nextLevel(), m_tgitm, m_joins.at(i), SelectQueryWhereComponent::WHERETYPE_ON);
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
    // firstly setting the table instance, it can have higher priority
    m_tinst = Workspace::getInstance()->workingVersion()->getTableInstance(tab);
    if(m_tinst == 0)
    {
        m_table = Workspace::getInstance()->workingVersion()->getTable(tab);
        if(m_table == 0)
        {
//            qDebug() << "Something went horribly wrong... no table and no table instance for: " << tab;
        }
    }
}

void TableQueryComponent::handleAction(const QString &action, QueryComponent *)
{
    if(action == ADD_ALIAS)
    {
        if(m_as == 0)
        {
            m_as = new SelectQueryAsComponent(getQuery(), this, nextLevel(), version());
            addChild(m_as);
            getQuery()->setQueryGlobalAlias(m_as, this);
            m_helper->triggerReRender();
        }
        return;
    }

    if(action == ADD_JOIN)
    {
        SelectQueryJoinComponent* join = new SelectQueryJoinComponent(getQuery(), this, nextLevel(), version());
        join->setHelper(m_helper);
        addChild(join);
        m_joins.append(join);
        m_helper->triggerReRender();

        return;
    }
    getParent()->handleAction(action, this);
}

CloneableElement* TableQueryComponent::clone(Version *sourceVersion, Version *targetVersion)
{
    TableQueryComponent* newc = m_tabInsteadOfTinst?new TableQueryComponent(getQuery(), m_table, getParent(), getLevel(), targetVersion)
                                                  :new TableQueryComponent(getQuery(), m_tinst, getParent(), getLevel(), targetVersion);

    newc->m_as = m_as?dynamic_cast<SelectQueryAsComponent*>(m_as->clone(sourceVersion, targetVersion)):0;
    cloneTheChildren(sourceVersion, targetVersion, newc);
    // Fetch the correct table from the new version

    if(m_tabInsteadOfTinst)
    {
        QString tableName = m_table->getName();
        const QVector<Table*> newVersionTables = targetVersion->getTables();
        for(int i=0; i<newVersionTables.size(); i++)
        {
            if(newVersionTables.at(i)->getName() == tableName)
            {
                newc->m_table = newVersionTables.at(i);
            }
        }
    }
    else
    {
        QString tableName = m_tinst->getName();
        const QVector<TableInstance*> newVersionTableInstances = targetVersion->getTableInstances();
        for(int i=0; i<newVersionTableInstances.size(); i++)
        {
            if(newVersionTableInstances.at(i)->getName() == tableName)
            {
                newc->m_tinst = newVersionTableInstances.at(i);
            }
        }
    }

    for(int i=0; i<m_joins.size(); i++)
    {
        SelectQueryJoinComponent* jI = dynamic_cast<SelectQueryJoinComponent*>(m_joins.at(i)->clone(sourceVersion, targetVersion));
        newc->m_joins.append(jI);
    }

    newc->setSourceUid(getObjectUid());
    return newc;
}

QueryComponent* TableQueryComponent::duplicate()
{
    TableQueryComponent* newc = m_tabInsteadOfTinst?new TableQueryComponent(getQuery(), m_table, getParent(), getLevel(), version())
                                                  :new TableQueryComponent(getQuery(), m_tinst, getParent(), getLevel(), version());

    newc->m_as = m_as?dynamic_cast<SelectQueryAsComponent*>(m_as->duplicate()):0;
    // TODO: qgh alias mapping

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
    getQuery()->deleteQueryGlobalAlias(this);
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
        int tinstSize = Workspace::getInstance()->workingVersion()->getTableInstances().size() ;
        if(tinstSize > 0)
        {
            int ctr = 0;
            while(ctr < tinstSize)
            {
                TableInstance* tinst = Workspace::getInstance()->workingVersion()->getTableInstances().at(ctr);
                if(tinst->table()->getColumnCount() > 0)
                {
                    tccp = new TableQueryComponent(parent->getQuery(), tinst, parent, level, parent->version());
                    break;
                }
                ctr ++;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        int tSize = Workspace::getInstance()->workingVersion()->getTables().size() ;
        if(tSize > 0)
        {
            int ctr = 0;
            while(ctr < tSize)
            {
                Table* t = Workspace::getInstance()->workingVersion()->getTables().at(ctr);
                if(t->getColumnCount() > 0)
                {
                    tccp = new TableQueryComponent(parent->getQuery(), t, parent, level, parent->version());
                    break;
                }
                ctr ++;
            }
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
    QStringList cs;
    QVector<const Column*> result;
    Table* tt = m_table;


    if(!m_tabInsteadOfTinst)
    {
        tt = m_tinst->table();
    }

    cs = tt->fullColumns();

    for(int i=0; i<cs.size(); i++)
    {
        const Column* c = tt->getColumn(cs.at(i));
        if(c == 0)
        {
            c = tt->getColumnFromParents(cs.at(i));
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
    QString result;
    if(m_tabInsteadOfTinst)
    {
        result = m_table->getName();
    }
    else
    {
        result = m_tinst->getName();
    }

    if(m_as)
    {
        result += m_as->get();
    }

    if(!m_joins.isEmpty())
    {
        for(int i=0; i<m_joins.size(); i++)
        {
            result += "\n";
            result += getSpacesForLevel();
            result += m_joins.at(i)->get();
        }
    }


    return result;
}
void TableQueryComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableElement = doc.createElement("Table");
    tableElement.setAttribute("tabInstedOfTinst", m_tabInsteadOfTinst);

    if(m_tabInsteadOfTinst)
    {
        tableElement.setAttribute("Name", m_table->getName());
    }
    else
    {
        tableElement.setAttribute("Name", m_tinst->getName());
    }

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

QUuid TableQueryComponent::getClassUid() const
{
    return QUuid(uidTableQueryComponent);
}

const Table* TableQueryComponent::getTable() const
{
    return m_table;
}

const TableInstance* TableQueryComponent::getTableInstance() const
{
    return m_tinst;
}
