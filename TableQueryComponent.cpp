#include "TableQueryComponent.h"
#include "Table.h"
#include "TableGraphicsItem.h"
#include "Workspace.h"
#include "Version.h"

TableQueryComponent::TableQueryComponent(Table* tab,QueryComponent* p, int level):QueryComponent(p, level), m_table(tab)
{
}

QString TableQueryComponent::get()
{
    return m_table->getName();
}

QueryGraphicsItem* TableQueryComponent::createGraphicsItem(QueryGraphicsHelper* helper, QueryGraphicsItem* parent)
{
    return new TableGraphicsItem(m_table, helper, parent, this);
}

void TableQueryComponent::setTable(const QString &tab)
{
    m_table = Workspace::getInstance()->workingVersion()->getTable(tab);
}
