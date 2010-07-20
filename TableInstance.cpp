#include "TableInstance.h"
#include "Table.h"

TableInstance::TableInstance(Table *tab) : TreeItem(), NamedItem(tab->getName()), m_table(tab), m_values()
{
    for(int i=0; i<m_table->fullColumns().size(); i++)
    {
        m_values.insert(m_table->fullColumns()[i], QVector<QString>());
    }
}

QList<QString> TableInstance::columns() const
{
    QList<QString> v = m_values.keys();
    return v;
}
