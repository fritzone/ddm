#include "qbr_Query.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_SelectQueryAsComponent.h"
#include "core_Table.h"
#include "core_TableInstance.h"

void Query::setQueryGlobalAlias(const SelectQueryAsComponent* as, const TableQueryComponent* tab)
{
    m_queryAliases[tab] = as;
}

void Query::deleteQueryGlobalAlias(const TableQueryComponent *tab)
{
    if(m_queryAliases.contains(tab))
    {
        m_queryAliases.remove(tab);
    }
}

QString Query::applyAlias(const QString &t)
{
    for(int i=0; i<m_queryAliases.keys().size(); i++)
    {
        const TableQueryComponent* tqc = m_queryAliases.keys()[i];
        QString tab;
        if(tqc->getTable() == 0)
        {
            tab = tqc->getTableInstance()->getName();
        }
        else
        {
            tab = tqc->getTable()->getName();
        }
        if(tab == t)
        {
            return m_queryAliases[m_queryAliases.keys()[i]]->getAs();
        }
    }
    return t;
}
