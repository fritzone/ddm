#include "core_TableInstance.h"
#include "db_AbstractSQLGenerator.h"
#include "core_Table.h"
#include "Configuration.h"
#include "Version.h"
#include "IconFactory.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"

TableInstance::TableInstance(Table *tab, bool ref, const QString& uid) : TreeItem(),
    NamedItem(tab->getName()), ObjectWithUid(uid),
    m_table(tab), m_values(), m_becauseOfReference(ref), m_referencingTables(0), m_instantiatedTablesInstances(), m_sentenced(false)
{
    for(int i=0; i<m_table->fullColumns().size(); i++)
    {
        m_values.insert(m_table->fullColumns()[i], QVector<QString>());
    }
    tab->addInstance(this);
}

TableInstance::TableInstance(const QString& name, bool ref, const QString &uid): TreeItem(),
    NamedItem(name), ObjectWithUid(uid),
    m_table(0), m_values(), m_becauseOfReference(ref), m_referencingTables(0), m_instantiatedTablesInstances(), m_sentenced(false)
{
}

void TableInstance::addTableReferencingThis(Table* refTab)
{
    m_referencingTables.append(refTab);
}

QVector<QString> TableInstance::columns() const
{
    QVector<QString> v;
    for(int i=0; i<m_table->fullColumns().size(); i++)
    {
        v.append(m_table->fullColumns()[i]);
    }
    return v;
}

void TableInstance::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableInstanceElement = doc.createElement("TableInstance");      // will hold the data in this element
    tableInstanceElement.setAttribute("Name", getName());
    tableInstanceElement.setAttribute("Table", m_table->getName());
    tableInstanceElement.setAttribute("Ref", instantiatedBecuaseOfRkReference());
    tableInstanceElement.setAttribute("uid", getObjectUid());
    tableInstanceElement.setAttribute("class-uid", getClassUid());
    tableInstanceElement.setAttribute("source-uid", getSourceUid());
    tableInstanceElement.setAttribute("locked", isLocked());
    tableInstanceElement.setAttribute("was-locked", wasLocked());

    {
    QString refTables = "";
    for(int i=0; i<m_referencingTables.size(); i++)
    {
        refTables += m_referencingTables.at(i)->getName();
        if(i<m_referencingTables.size()-1)
        {
            refTables += ",";
        }
    }
    tableInstanceElement.setAttribute("ReferencingTables", refTables);
    }

    {
    QString instantiatedTablesInstances = "";
    for(int i=0; i<m_instantiatedTablesInstances.size(); i++)
    {
        instantiatedTablesInstances += m_instantiatedTablesInstances.at(i)->getName();
        if(i<m_instantiatedTablesInstances.size()-1)
        {
            instantiatedTablesInstances += ",";
        }
    }
    tableInstanceElement.setAttribute("InstantiatedTableInstances", instantiatedTablesInstances);
    }

    QVector<QString> cols = columns();

    for(int i=0; i<cols.size(); i++)
    {
        QDomElement column = doc.createElement("Column");
        column.setAttribute("Name", cols[i]);
        QVector<QString> colVs = m_values[cols[i]];
        for(int j=0; j<colVs.size(); j++)
        {
            QDomElement value = doc.createElement("Value");
            value.setAttribute("value", colVs[j]);
            column.appendChild(value);
        }
        tableInstanceElement.appendChild(column);
    }

    parent.appendChild(tableInstanceElement);
}

QStringList TableInstance::generateSqlSource(AbstractSqlGenerator *generator, QHash<QString,QString> opts, const Connection *dest)
{
    QStringList result;
    m_table->restartSqlRendering();
    result << generator->generateCreateTableSql(m_table, opts, getName(), dest);
    // and now the default values as "inserts"
    result << generator->generateDefaultValuesSql(this, opts);
    return result;
}

void TableInstance::addColumn(const QString& colName)
{
    m_values.insert(colName, QVector<QString>());
}

QVector<QString> TableInstance::removeColumn(const QString &colName)
{
    QVector<QString> result = m_values[colName];
    m_values.erase(m_values.find(colName));
    return result;
}

void TableInstance::renameColumn(const QString& oldName, const QString& newName)
{
    QVector<QString> tmp = removeColumn(oldName);
    m_values.insert(newName, tmp);
}

QUuid TableInstance::getClassUid() const
{
    return QUuid(uidTableInstance);
}

CloneableElement* TableInstance::clone(Version *sourceVersion, Version *targetVersion)
{
    TableInstance* result = new TableInstance(getName(), m_becauseOfReference, QUuid::createUuid().toString());
    result->setSourceUid(getObjectUid());
    result->m_values = m_values;

    return result;
}


void TableInstance::finalizeCloning(TableInstance *src, Version *sourceVersion, Version *targetVersion)
{
    Table* otherTable = src->table();
    Table* myTable = targetVersion->getTable(otherTable->getName());
    myTable->addInstance(this);
    if(!myTable) return;
    m_table = myTable;

    for(int i=0; i<src->m_referencingTables.size(); i++)
    {
        Table* otherRefTable = src->m_referencingTables.at(i);
        Table* myRefTable = targetVersion->getTable(otherRefTable->getName());
        if(!myRefTable) return;
        m_referencingTables.append(myRefTable);
    }
}

void TableInstance::finalizeAutoinstantiatedTinsts(TableInstance* src, Version *sourceVersion, Version *targetVersion)
{
    for(int i=0; i<src->m_instantiatedTablesInstances.size(); i++)
    {
        TableInstance* myInstantiated = targetVersion->getTableInstance(src->m_instantiatedTablesInstances.at(i)->getName());
        if(!myInstantiated) return;
        m_instantiatedTablesInstances.append(myInstantiated);
    }
}

void TableInstance::updateGui()
{
    if(isLocked())
    {
        getLocation()->setIcon(0, IconFactory::getLockedTabinstIcon());
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockTableInstancePopupMenu());
    }
    else
    {
        if(wasLocked())
        {
            // set the icon, add to the tree
            if(instantiatedBecuaseOfRkReference())
            {
                getLocation()->setIcon(0, IconFactory::getTabinstLockIcon());
            }
            else
            {
                getLocation()->setIcon(0, IconFactory::getTabinstIcon());
            }
            getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getRelockTableInstancePopupMenu());
        }
    }
    TreeItem::updateGui();
}
