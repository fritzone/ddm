#include "TableQueryComponent.h"
#include "Table.h"
#include "TableGraphicsItem.h"
#include "Workspace.h"
#include "Version.h"
#include "strings.h"
#include "SelectQueryAsComponent.h"
#include "CellAsCommand.h"

TableQueryComponent::TableQueryComponent(Table* tab, QueryComponent* p, int level):QueryComponent(p, level), m_table(tab), m_as(0)
{
}

QString TableQueryComponent::get()
{
    return m_table->getName();
}

QueryGraphicsItem* TableQueryComponent::createGraphicsItem(QueryGraphicsHelper* helper, QueryGraphicsItem* parent)
{
    TableGraphicsItem* tgitm = new TableGraphicsItem(m_table, helper, parent, this);
    if(m_as)
    {
        tgitm->setAs(new CellAsCommand(helper, m_level + 1, tgitm, m_as));
    }
    return tgitm;
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
        }
        return;
    }
    m_parent->handleAction(action, this);
}

QueryComponent* TableQueryComponent::duplicate()
{
    TableQueryComponent* newc = new TableQueryComponent(m_table, m_parent, m_level);
    newc->m_as = m_as;

    return newc;
}

QSet<OptionsType> TableQueryComponent::provideOptions()
{
    QSet<OptionsType> result;
    result.insert(OPTIONS_ADD_JOIN);
    result.insert(OPTIONS_AS);
    return result;
}
