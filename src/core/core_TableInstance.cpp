#include "core_TableInstance.h"
#include "db_AbstractSQLGenerator.h"
#include "core_Table.h"
#include "Configuration.h"
#include "Version.h"
#include "IconFactory.h"
#include "IconFactory.h"
#include "core_Column.h"
#include "ContextMenuCollection.h"
#include "ForeignKey.h"
#include "core_Trigger.h"

TableInstance::TableInstance(Table *tab, bool ref, const QString& uid, Version *v) : TreeItem(),
    NamedItem(tab->getName()), VersionElement(uid, v),
    m_table(tab), m_values(), m_becauseOfReference(ref), m_referencingTables(0), m_instantiatedTablesInstances(), m_sentenced(false), m_tabName()
{
    for(int i=0; i<m_table->fullColumns().size(); i++)
    {
        m_values.insert(m_table->fullColumns()[i], QVector<QString>());
    }
    tab->addInstance(this);
}

TableInstance::TableInstance(const QString& name, bool ref, const QString &uid, Version *v): TreeItem(),
    NamedItem(name), VersionElement(uid, v),
    m_table(0), m_values(), m_becauseOfReference(ref), m_referencingTables(0), m_instantiatedTablesInstances(), m_sentenced(false), m_tabName()
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
    tableInstanceElement.setAttribute("locked", lockState() == LockableElement::LOCKED);
    tableInstanceElement.setAttribute("was-locked", wasLocked());

    // The referencing tables
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

    // The Instantiated Table Instances
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

    // The Columns
    {
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
    }

    // The Foreign Key Mappings
    {
        QDomElement fkMap = doc.createElement("FkMappings");
        for(int i=0; i<m_fkMappings.keys().size(); i++)
        {
            QDomElement fkEntry = doc.createElement("FkEntry");
            fkEntry.setAttribute("FK", m_fkMappings.keys().at(i));
            fkEntry.setAttribute("TINST", m_fkMappings[m_fkMappings.keys().at(i)]);
            fkMap.appendChild(fkEntry);
        }
        tableInstanceElement.appendChild(fkMap);
    }
    parent.appendChild(tableInstanceElement);
}

QStringList TableInstance::generateSqlSource(AbstractSqlGenerator *generator, QHash<QString,QString> opts, const Connection *dest)
{
    QStringList result;
    m_table->restartSqlRendering();
    result << generator->generateCreateTableSql(m_table, opts, getName(), m_fkMappings, dest);
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

CloneableElement* TableInstance::clone(Version */*sourceVersion*/, Version *targetVersion)
{
    TableInstance* result = new TableInstance(getName(), m_becauseOfReference, QUuid::createUuid().toString(), targetVersion);
    result->setSourceUid(getObjectUid());
    result->m_values = m_values;
    result->m_fkMappings = m_fkMappings;
    return result;
}


void TableInstance::finalizeCloning(TableInstance *src, Version */*sourceVersion*/, Version *targetVersion)
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

void TableInstance::finalizeAutoinstantiatedTinsts(TableInstance* src, Version */*sourceVersion*/, Version *targetVersion)
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
    if(lockState() == LockableElement::LOCKED)
    {
        getLocation()->setIcon(0, IconFactory::getLockedTabinstIcon());
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockTableInstancePopupMenu());
    }
    else
    if(lockState() == LockableElement::FINAL_LOCK)
    {
        getLocation()->setIcon(0, IconFactory::getFinalLockedTabinstIcon());
        getLocation()->setPopupMenu(0);
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

/**
 * @brief TableInstance::getValues
 * @param columns
 * @return
 */
QVector <QVector<ColumnWithValue*> > TableInstance::getValues(QVector<ColumnWithValue*> columns)
{
    QVector<ColumnWithValue*> vecPks;
    int ctr = 0;
    int size = 0;
    while(true)
    {
        for(int i=0; i<columns.size(); i++)
        {
            Column* c = columns.at(i)->column;
            const QVector<QString>& fromColVs = values()[c->getName()];
            size = size>fromColVs.size()?size:fromColVs.size();
            if(fromColVs.isEmpty()) continue;

            if(fromColVs[ctr] == columns.at(i)->value || columns.at(i)->value.isEmpty())
            {
                ColumnWithValue* ftcWv = new ColumnWithValue;
                ftcWv->column = c;
                ftcWv->value = fromColVs[ctr];
                vecPks.append(ftcWv);
            }
            else
            {
                ColumnWithValue* ftcWv = new ColumnWithValue;
                ftcWv->column = c;
                ftcWv->value = "DDM:IGNORE_THIS_ROW";
                vecPks.append(ftcWv);
            }
        }
        ctr++;
        if(ctr >= size)
        {
            break;
        }
    }

    QVector <QVector<ColumnWithValue*> >  result;
    QVector<ColumnWithValue*> row;
    for(int i=0; i<vecPks.size(); i++)
    {
        if(i%columns.size() == 0)
        {
            if(!row.isEmpty())
            {
                // see if there is a DDM ignore entry in this row
                bool ignorable = false;
                for(int j=0; j<row.size(); j++)
                {
                    if(row.at(j)->value == "DDM:IGNORE_THIS_ROW") ignorable = true;
                }
                if(!ignorable)
                {
                    result.append(row);
                }
            }
            row.clear();
        }
        ColumnWithValue* cwv = new ColumnWithValue;
        cwv->column = vecPks.at(i)->column;
        cwv->value = vecPks.at(i)->value;
        row.append(cwv);
    }
    result.append(row);
    for(int i=0; i<result.size(); i++)
    {
        const QVector<ColumnWithValue*> row = result[i];
        QString x = "";
        for(int j=0; j<row.size(); j++)
        {
            x += row.at(j)->column->getName() + "="  + row.at(j)->value + "  ";
        }
    }

    return result;
}

QVector <QVector<ColumnWithValue*> > TableInstance::getFullValues()
{
    QVector<ColumnWithValue*> vecPks;
    int ctr = 0;
    int size = 0;
    while(true)
    {
        for(int i=0; i<columns().size(); i++)
        {
            Column* c = table()->getColumn(columns().at(i));
            if(!c) c = table()->getColumnFromParents(columns().at(i));
            if(!c) continue;

            const QVector<QString>& fromColVs = values()[c->getName()];
            size = size>fromColVs.size()?size:fromColVs.size();
            if(fromColVs.isEmpty()) continue;

            ColumnWithValue* ftcWv = new ColumnWithValue;
            ftcWv->column = c;
            ftcWv->value = fromColVs[ctr];
            vecPks.append(ftcWv);
        }
        ctr++;
        if(ctr >= size)
        {
            break;
        }
    }

    QVector <QVector<ColumnWithValue*> >  result;
    QVector<ColumnWithValue*> row;
    for(int i=0; i<vecPks.size(); i++)
    {
        if(i%columns().size() == 0)
        {
            if(!row.isEmpty())
            {
                result.append(row);
            }
            row.clear();
        }
        ColumnWithValue* cwv = new ColumnWithValue;
        cwv->column = vecPks.at(i)->column;
        cwv->value = vecPks.at(i)->value;
        row.append(cwv);
    }
    result.append(row);
    for(int i=0; i<result.size(); i++)
    {
        const QVector<ColumnWithValue*> row = result[i];
        QString x = "";
        for(int j=0; j<row.size(); j++)
        {
            x += row.at(j)->column->getName() + "="  + row.at(j)->value + "  ";
        }
    }

    return result;
}

void TableInstance::setFkMappingToTinst(const QString& fkName, const QString& destTinst)
{
    m_fkMappings[fkName] = destTinst;
}

QString TableInstance::getReferencingTables() const
{
    QString result = "";
    for(int i=0; i<m_referencingTables.size(); i++)
    {
        result += m_referencingTables.at(i)->getName();
        result += " ";
    }
    return result;
}

bool TableInstance::finalizeFkMappings(QVector<QString>& failedFks)
{
    Table* t = table();
    bool result = false;
    const QVector<ForeignKey*> &fks = t->getForeignKeys();
    for(int i=0; i<fks.size(); i++)
    {
        const QVector<TableInstance*> &otherTinsts = version()->getTableInstances();
        QString first = "";

        for(int j=0; j<otherTinsts.size(); j++)
        {
            TableInstance* tabInstJ = otherTinsts.at(j);
            if(tabInstJ->table()->getObjectUid() == fks[i]->getForeignTable()->getObjectUid())  // same tab
            {
                if(first.isEmpty()) first = tabInstJ->getName();
            }
        }
        if(!first.isEmpty())
        {
            if(!hasFkMappingFor(fks[i]->getName()))
            {
                setFkMappingToTinst(fks[i]->getName(), first);
            }
            else
            {
                QString tabToFkMap = getTinstForFk(fks[i]->getName());
                TableInstance* otherTinst = version()->getTableInstance(tabToFkMap);
                if(otherTinst == 0) // the table instance went missing, default it to "first"
                {
                    setFkMappingToTinst(fks[i]->getName(), first);
                }
            }
            result = true;
        }
        else
        {
            failedFks.append(fks[i]->getName());
            result = false;
        }

    }
    return result;
}

void TableInstance::updateFksDueToTInstRename(const QString& oldName, const QString& newName)
{
    for(int i=0; i<m_fkMappings.keys().size(); i++)
    {
        if(m_fkMappings[m_fkMappings.keys()[i]] == oldName)
        {
            m_fkMappings[m_fkMappings.keys()[i]] = newName;
        }
    }
}

void TableInstance::onRename(const QString &oldName, const QString &newName)
{
    // walk through the version, and for all the other table instances call the updateFksDueToTInstRename();
    const QVector<TableInstance*> otherInstances = version()->getTableInstances();
    for(int i=0; i<otherInstances.size(); i++)
    {
        if(otherInstances[i]->getObjectUid() != getObjectUid())
        {
            otherInstances[i]->updateFksDueToTInstRename(oldName, newName);
        }
    }

    // and the same for all the Triggers
    const QVector<Trigger*> triggers = version()->getTriggers();
    for(int i=0; i<triggers.size(); i++)
    {
        if(triggers[i]->getTable() == oldName)
        {
            triggers[i]->setName(newName);
        }
    }
}
