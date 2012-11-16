#include "core_TableInstance.h"
#include "db_AbstractSQLGenerator.h"
#include "core_Table.h"
#include "Configuration.h"
#include "Version.h"
#include "IconFactory.h"
#include "IconFactory.h"
#include "core_Column.h"
#include "ContextMenuCollection.h"

TableInstance::TableInstance(Table *tab, bool ref, const QString& uid, Version *v) : TreeItem(),
    NamedItem(tab->getName()), ObjectWithUid(uid, v),
    m_table(tab), m_values(), m_becauseOfReference(ref), m_referencingTables(0), m_instantiatedTablesInstances(), m_sentenced(false), m_tabName()
{
    for(int i=0; i<m_table->fullColumns().size(); i++)
    {
        m_values.insert(m_table->fullColumns()[i], QVector<QString>());
    }
    tab->addInstance(this);
}

TableInstance::TableInstance(const QString& name, bool ref, const QString &uid, Version *v): TreeItem(),
    NamedItem(name), ObjectWithUid(uid, v),
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
    TableInstance* result = new TableInstance(getName(), m_becauseOfReference, QUuid::createUuid().toString(), targetVersion);
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
        if(ctr == size)
        {
            break;
        }
    }

    QVector <QVector<ColumnWithValue*> >  result;
    QVector<ColumnWithValue*> row;
    for(int i=0; i<vecPks.size(); i++)
    {
        if(i%size == 0)
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
    qDebug() << "RESULT:";
    result.append(row);
    for(int i=0; i<result.size(); i++)
    {
        const QVector<ColumnWithValue*> row = result[i];
        for(int j=0; j<row.size(); j++)
        {
            qDebug() << row.at(j)->column->getName() << " = " << row.at(j)->value;
        }
    }
}

QVector <QVector<ColumnWithValue*> > TableInstance::getFullValues(QVector<ColumnWithValue*> cols)
{
    QVector<ColumnWithValue*> vecPks;
    int ctr = 0;
    int size = 0;
    while(true)
    {
        for(int i=0; i<cols.size(); i++)
        {
            Column* c = cols.at(i)->column;
            const QVector<QString>& fromColVs = values()[c->getName()];
            size = size>fromColVs.size()?size:fromColVs.size();
            ColumnWithValue* ftcWv = new ColumnWithValue;
            ftcWv->column = c;
            ftcWv->value = fromColVs[ctr];
            vecPks.append(ftcWv);
        }
        ctr++;
        if(ctr == size)
        {
            break;
        }
    }

    QVector <QVector<ColumnWithValue*> >  result;
    QVector<ColumnWithValue*> row;
    for(int i=0; i<vecPks.size(); i++)
    {
        if(i%size == 0)
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
    qDebug() << "RESULT:";
    result.append(row);
    for(int i=0; i<result.size(); i++)
    {
        const QVector<ColumnWithValue*> row = result[i];
        for(int j=0; j<row.size(); j++)
        {
            qDebug() << row.at(j)->column->getName() << " = " << row.at(j)->value;
        }
    }
}
