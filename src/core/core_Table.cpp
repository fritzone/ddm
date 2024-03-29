#include "core_Table.h"
#include "core_Column.h"
#include "core_Index.h"
#include "Version.h"
#include "core_ForeignKey.h"
#include "db_DatabaseEngine.h"
#include "Configuration.h"
#include "core_UserDataType.h"
#include "db_AbstractSQLGenerator.h"
#include "IconFactory.h"
#include "NameGenerator.h"
#include "core_TableInstance.h"
#include "Workspace.h"
#include "Version.h"
#include "uids.h"
#include "UidWarehouse.h"
#include "ddm_strings.h"
#include "ContextMenuCollection.h"
#include <QApplication>
#include <QClipboard>

Table::Table(Version* v, QString uid) : SerializableElement(), SqlSourceTreeItem(), CopyableElement(),
    NamedItem(NameGenerator::getUniqueName(v, (itemGetter)&Version::getTable, QString("TAB"))),
    VersionElement(uid, v), ObjectWithSpInstances(), TreeItem(),
    m_description(""), m_columns(), m_indices(), m_foreignKeys(), m_startupValues(),
    m_parent(0),
    m_version(v), m_children()
{
}

void Table::addColumn(Column *column)
{
    m_columns.append(column);
    column->setTable(this);
}

void Table::addIndex(Index *index)
{
    m_indices.append(index);
}

void Table::moveColumnDown(int c)
{
    if(c < m_columns.size() - 1)
    {
        Column* tc = m_columns[c];
        m_columns[c] = m_columns[c+1];
        m_columns[c+1] = tc;
    }
}

void Table::moveColumnUp(int c)
{
    if(c > 0)
    {
        Column* tc = m_columns[c];
        m_columns[c] = m_columns[c-1];
        m_columns[c-1] = tc;
    }
}

bool Table::hasIndex(const QString& indexName) const
{
    for(int i=0; i<m_indices.size(); i++)
    {
        if(m_indices[i]->getName().toUpper() == indexName.toUpper())
        {
            return true;
        }
    }
    return false;
}

bool Table::hasColumn(const QString& colName) const
{
    for(int i=0; i<m_columns.size(); i++)
    {
        if(m_columns[i]->getName().toUpper() == colName.toUpper())
        {
            return true;
        }
    }
    return false;
}

int Table::getTotalParentColumnCount() const
{
    int tc = 0;
    const Table* p = m_parent;
    while(p)
    {
        tc += p->getColumnCount();
        p = p->m_parent;
    }
    return tc;

}


bool Table::parentsHaveColumn(const QString& colName) const
{
    const Table* p = m_parent;
    while(p)
    {
        if(p->hasColumn(colName))
        {
            return true;
        }
        p = p->m_parent;
    }
    return false;
}

Index* Table::getIndexFromParents(const QString& indexName) const
{
    const Table* p = m_parent;
    while(p)
    {
        if(p->getIndex(indexName))
        {
            return p->getIndex(indexName);
        }
        p = p->m_parent;
    }
    return 0;
}

ForeignKey* Table::getForeignKeyFromParents(const QString& fkName) const
{
    const Table* p = m_parent;
    while(p)
    {
        if(p->getForeignKey(fkName))
        {
            return p->getForeignKey(fkName);
        }
        p = p->m_parent;
    }
    return 0;
}

Index* Table::getIndex(const QString& idxName) const
{
    for(int c=0; c<m_indices.size(); c++)
    {
        if(m_indices.at(c)->getName() == idxName)
        {
            return m_indices[c];
        }
    }
    return 0;
}

ForeignKey* Table::getForeignKey(const QString& name) const
{
    for(int i=0; i<m_foreignKeys.size(); i++)
    {
        if(m_foreignKeys[i]->getName().toUpper() == name.toUpper())
        {
            return m_foreignKeys[i];
        }
    }
    return 0;
}

ForeignKey* Table::getForeignKeyToTable(const QString& tableName) const
{
    for(int i=0; i<m_foreignKeys.size(); i++)
    {
        if(m_foreignKeys[i]->getForeignTableName().toUpper() == tableName.toUpper())
        {
            return m_foreignKeys[i];
        }
    }
    return 0;
}

ForeignKey* Table::getForeignKeyToTable(Table* tab) const
{
    for(int i=0; i<m_foreignKeys.size(); i++)
    {
        if(m_foreignKeys[i]->getForeignTable() == tab)
        {
            return m_foreignKeys[i];
        }
    }
    return 0;
}


Column* Table::getColumn(const QString& name) const
{
    for(int i = 0; i<m_columns.size(); i++)
    {
        if(m_columns[i]->getName().toUpper() == name.toUpper())
        {
            return m_columns[i];
        }
    }
    return 0;
}

Column* Table::getColumnByUid(const QString& uid) const
{
    for(int i = 0; i<m_columns.size(); i++)
    {
        if(m_columns[i]->getObjectUid() == uid)
        {
            return m_columns[i];
        }
    }
    return 0;
}

Column* Table::getColumnFromParents(const QString& name) const
{
    const Table* p = m_parent;
    while(p)
    {
        Column* c = p->getColumn(name);
        if(c)
        {
            return c;
        }
        p = p->m_parent;
    }
    return 0;
}


void Table::removeIndex(Index* toRemove)
{
    int idx = m_indices.indexOf(toRemove);
    if(idx != -1)
    {
        m_indices.remove(idx);
    }
}

void Table::removeColumn(Column* toRemove)
{
    int idx = m_columns.indexOf(toRemove);
    if(idx != -1)
    {
        m_columns.remove(idx);
    }
}

void Table::removeForeignKey(ForeignKey* toRemove)
{
    int idx = m_foreignKeys.indexOf(toRemove);
    if(idx != -1)
    {
        m_foreignKeys.remove(idx);
    }
}


Index* Table::isColumnUsedInIndex(const Column *column)
{
    for(int i=0; i<m_indices.size(); i++)
    {
        if(m_indices[i]->hasColumn(column))
        {
            return m_indices[i];
        }
    }
    return 0;
}

QStringList Table::columns() const
{
    QStringList result;
    for(int i=0; i<m_columns.size(); i++)
    {
        result << m_columns[i]->getName();
    }
    return result;
}

QStringList Table::indices() const
{
    QStringList result;
    for(int i=0; i<m_indices.size(); i++)
    {
        result << m_indices[i]->getName();
    }
    return result;

}

QStringList Table::fullColumns() const
{
    QStringList result;

    const Table* p = m_parent;
    while(p)
    {
        result << p->columns();
        p = p->m_parent;
    }


    for(int i=0; i<m_columns.size(); i++)
    {
        result << m_columns[i]->getName();
    }
    return result;
}

QStringList Table::fullIndices() const
{
    QStringList result;

    const Table* p = m_parent;
    while(p)
    {
        result << p->indices();
        p = p->m_parent;
    }


    for(int i=0; i<indices().size(); i++)
    {
        result << indices()[i];
    }
    return result;
}

void Table::setParentUid(const QString & s)
{
    m_parentUid = s;
}

void Table::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableElement = doc.createElement("Table");      // will hold the data in this element
    tableElement.setAttribute("Name", m_name);
    tableElement.setAttribute("Parent",m_parent?m_parent->getName():strNA);
    tableElement.setAttribute("uid", getObjectUid().toString());
    tableElement.setAttribute("class-uid", getClassUid().toString());
    tableElement.setAttribute("parent-uid", m_parent?m_parent->getObjectUid().toString():strNA);

    tableElement.setAttribute("locked", lockState() == LockableElement::LOCKED);
    tableElement.setAttribute("source-uid", getSourceUid().toString().length() ? getSourceUid().toString() : strNA);
    tableElement.setAttribute("was-locked", wasLocked());

    if(m_parent !=0)
    {
        tableElement.setAttribute("Parent", m_parent->getName());
    }

    {
    QDomElement descElement = doc.createElement("Description");  // description
    QDomText descNode = doc.createTextNode(m_description);
    descElement.appendChild(descNode);
    tableElement.appendChild(descElement);
    }

    // save the columns
    {
    QDomElement columnsElement = doc.createElement("Columns");
    for(int i=0; i<m_columns.size(); i++)
    {
        m_columns[i]->serialize(doc, columnsElement);
    }
    tableElement.appendChild(columnsElement);
    }

    // save the indices
    {
    QDomElement indicesElement = doc.createElement("Indices");
    for(int i=0; i<m_indices.size(); i++)
    {
        m_indices[i]->serialize(doc, indicesElement);
    }
    tableElement.appendChild(indicesElement);
    }

    // save the foreign keys
    {
    QDomElement fksElement = doc.createElement("ForeignKeys");
    for(int i=0; i<m_foreignKeys.size(); i++)
    {
        m_foreignKeys[i]->serialize(doc, fksElement);
    }
    tableElement.appendChild(fksElement);
    }

    // save the default values
    {
    QDomElement startupsElement = doc.createElement("StartupValues");
    for(int i=0; i<m_startupValues.size(); i++)
    {
        QDomElement row = doc.createElement("Row");
        const QVector<QString> &rowI = m_startupValues[i];
        for(int j=0; j<rowI.size(); j++)
        {
            QDomElement column = doc.createElement("Data");
            column.setAttribute("Type", m_columns[j]->getDataType()->getName());
            column.setAttribute("Value", rowI[j]);
            row.appendChild(column);
        }
        startupsElement.appendChild(row);
    }
    tableElement.appendChild(startupsElement);

    }

    // save the sps
    {
    QDomElement spsElement = doc.createElement("SpInstances");
    serialize_spinstances(doc, spsElement);
    tableElement.appendChild(spsElement);
    }
    parent.appendChild(tableElement);
}

void Table::setDefaultValues(QVector <QVector <QString> > & values)
{
    m_startupValues = values;
}

const UserDataType* Table::getDataTypeOfColumn(const QString& cname) const
{
    Column* col = getColumn(cname);
    if(!col)
    {
        col = getColumnFromParents(cname);
        if(!col)
        {
            return 0; // Shouldn't ever happen
        }
    }
    return col->getDataType();
}

QStringList Table::generateSqlSource(AbstractSqlGenerator *generator, QHash<QString,QString> opts, const Connection* dest)
{
    const_cast<Table*>(this)->restartSqlRendering();
    QStringList createSql = generator->generateCreateTableSql(const_cast<Table*>(this), opts, getName(), QMap<QString, QString>(), dest);
    if(!Workspace::getInstance()->currentProjectIsOop())
    {
        createSql << generator->generateDefaultValuesSql(this, opts);
    }
    return createSql;
}

QSet<const Table*> Table::getTablesReferencedByForeignKeys()
{
    QSet<const Table*> result;
    for(int i=0; i<m_foreignKeys.size(); i++)
    {
        for(int j=0; j<m_foreignKeys.at(i)->getAssociations().size(); j++)
        {
            result.insert(m_foreignKeys.at(i)->getAssociations().at(j)->getForeignTable());
        }
    }
    return result;
}

QString Table::getAvailableIndexName(const QString& prefix)
{
    QString lprefix = prefix;
    for(int i=0; i<999999; i++)
    {
        lprefix = prefix + "_" + QString::number(i);
        if(getIndex(lprefix) == 0)
        {
            return lprefix;
        }
    }
    return "None";
}

Index* Table::createAutoIndex(QVector<const Column*> cols, bool dueTofk)
{
    QString idxName = getAvailableIndexName(dueTofk?strAutoIdxFk:strAutoIdx + strUnderline + getName());
    Index* idx = new Index(idxName, this, QUuid::createUuid().toString(), version());
    bool onlyPks = true;
    for(int i=0; i<cols.size(); i++)
    {
        if(!cols[i]->isPk())
        {
            onlyPks = false;
        }
        idx->addColumn(cols.at(i), "");
    }

    if(onlyPks) // this will create the index only for NON primary key columns
    {
        delete idx;
        return 0;
    }
    addIndex(idx);
    return idx;
}

void Table::tableInstancesAddColumn(Column* col)
{
    for(int i=0; i<m_tableInstances.size(); i++)
    {
        m_tableInstances.at(i)->addColumn(col->getName());
    }
}

void Table::tableInstancesRenameColumn(const QString &oldName, const QString &newName)
{
    for(int i=0; i<m_tableInstances.size(); i++)
    {
        m_tableInstances.at(i)->renameColumn(oldName, newName);
    }
}


void Table::tableInstancesRemoveColumn(Column* col)
{
    for(int i=0; i<m_tableInstances.size(); i++)
    {
        m_tableInstances.at(i)->removeColumn(col->getName());
    }
}

void Table::copy()
{
    QDomDocument doc("ClipboardData");
    QDomElement root = doc.createElement("CopiedTable");
    root.setAttribute("version", version()->getObjectUid().toString());
    serialize(doc, root);
    doc.appendChild(root);

    QString text = doc.toString();

    QApplication::clipboard()->setText(text);
}


QVector<ForeignKey*> Table::columnParticipatesInForeignKey(const Column* col)
{
    QVector<ForeignKey*> result;
    for(int i=0; i<m_foreignKeys.size(); i++)
    {
        QVector<const Column*> localColumns = m_foreignKeys.at(i)->localColumns();
        if(localColumns.contains(col))
        {
            result.append(m_foreignKeys.at(i));
        }
    }
    return result;
}

QString Table::generateUniqueColumnName(const QString& in)
{
    int i = 1;
    if(!hasColumn(in)) return in;
    while(i<999999)
    {
        QString result2 = in + "_" + QString::number(i);
        if(!hasColumn(result2)) return result2;
        i++;
    }
    return in;
}

void Table::addSpecializedTable(const Table* childTable)
{
    m_children.append(childTable);
}

bool Table::hasSpecializedTables() const
{
    return m_children.size() > 0;
}

void Table::removeSpecializedTable(Table* childTable)
{
    m_children.remove(m_children.indexOf(childTable));
}

void Table::setParent(Table* parent)
{
    m_parent = parent;
    if(parent)
    {
        m_parent->addSpecializedTable(this);
    }
}

Table* Table::getParent() const
{
    return m_parent;
}

QUuid Table::getClassUid() const
{
    return QUuid(uidTable);
}

CloneableElement* Table::clone(Version *sourceVersion, Version *targetVersion)
{
    Table* result = new Table(targetVersion, QUuid::createUuid().toString());
    result->setDescription(m_description);
    result->m_startupValues = m_startupValues;  // this is plainly copyable

    result->setSourceUid(getObjectUid());

    cloneSps(result);

    // now fix the columns
    const QVector<Column*> cols = getColumns();
    for(int i=0; i<cols.size(); i++)
    {
        Column* newColumn = dynamic_cast<Column*>(cols.at(i)->clone(sourceVersion, targetVersion));
        result->addColumn(newColumn);
    }

    // now fix the indexes
    const QVector<Index*> idxs = getIndices();
    for(int i=0; i<idxs.size(); i++)
    {
        Index* indx = dynamic_cast<Index*>(idxs.at(i)->clone(sourceVersion, targetVersion));
        result->addIndex(indx);
        indx->finalizeCloning(result, idxs.at(i));
    }

    return result;
}

void Table::updateGui()
{
    if(lockState() == LockableElement::LOCKED)
    {
        getLocation()->setIcon(0, IconFactory::getLockedTableIcon());
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockTablePopupMenu());
    }
    else
    if(lockState() == LockableElement::FINAL_LOCK)
    {
        getLocation()->setIcon(0, IconFactory::getFinalLockedTableIcon());
        getLocation()->setPopupMenu(0);
    }
    else
    {
        if(wasLocked())
        {
            getLocation()->setIcon(0, IconFactory::getTableIcon());
            getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getRelockTablePopupMenu());
        }
    }
    TreeItem::updateGui();
}

int Table::getIndexOfColumn(const Column *c)
{
    const QVector<Column*> cols = getColumns();

    for(int i=0; i<cols.size(); i++)
    {
        if(c->getObjectUid() == cols[i]->getObjectUid())
        {
            return i;
        }
    }
    return -1;
}

int Table::getIndexOfColumn(const QString& n)
{
    const QVector<Column*> cols = getColumns();

    for(int i=0; i<cols.size(); i++)
    {
        if(n.toUpper() == cols[i]->getName().toUpper())
        {
            return i;
        }
    }
    return -1;
}


QStringList Table::primaryKeyColumnsAsStringlist() const
{
    QStringList primaryKeys;
    for(int i=0; i<fullColumns().size(); i++)
    {
        Column *col = getColumn(fullColumns()[i]);
        if(col == 0)
        {
            col = getColumnFromParents(fullColumns()[i]);
            if(col == 0)
            {
                //qDebug() << "Cannot get a column:" << fullColumns()[i] << "from"
                //         << getName();
                return primaryKeys;
            }
        }

        if(col->isPk())
        {
            primaryKeys.append(fullColumns()[i]);
        }
    }

    return primaryKeys;
}

QSet<Column*> Table::primaryKeyColumnsAsSet() const
{
    QSet<Column*> result;
    QStringList fromCols = fullColumns();
    for(int i=0; i<fromCols.size(); i++)
    {
        Column* c = getColumn(fromCols[i]);
        if(!c) c = getColumnFromParents(fromCols[i]);
        if(!c) continue;

        if(c->isPk()) result.insert(c);
    }
    return result;
}

QVector <QVector<ColumnWithValue*> > Table::getFullValues() const
{
    QVector <QVector<ColumnWithValue*> > result;
    const QVector <QVector <QString> >& sv = getDefaultValues();
    for(int i=0; i<sv.size(); i++)
    {
        const QVector<QString> &rowI = sv[i];
        QVector<ColumnWithValue*> resI;
        for(int j=0; j<rowI.size(); j++)
        {
            ColumnWithValue* cwv = new ColumnWithValue;
            cwv->column = getColumns().at(j);
            cwv->value = rowI[j];
            resI.append(cwv);
        }
        result.append(resI);
    }
    return result;
}

QVector <QVector<ColumnWithValue*> > Table::getValues(QVector<ColumnWithValue*> vals) const
{
    QVector <QVector<ColumnWithValue*> > result;
    const QVector <QVector <QString> >& sv = getDefaultValues();
    for(int i=0; i<sv.size(); i++)
    {
        const QVector<QString> &rowI = sv[i];
        QVector<ColumnWithValue*> resI;
        for(int j=0; j<rowI.size(); j++)
        {
            Column* c = getColumns().at(j);
            // now see if the column c is in the vals
            bool found = false;
            for(int k = 0; k<vals.size(); k++)
            {
                if(vals[k]->column->getName() == c->getName())
                {
                    found = true;
                    break;
                }
            }
            if(found)
            {
                ColumnWithValue* cwv = new ColumnWithValue;
                cwv->column = c;
                cwv->value = rowI[j];
                resI.append(cwv);
            }
        }
        result.append(resI);
    }
    return result;

}

Column* Table::getDescendantColumn(const Column* sourceColumn)
{
    const QStringList &cols = fullColumns();
    for(int i=0; i<cols.size(); i++)
    {
        Column *c = getColumn(cols[i]);
        if(c == 0) c = getColumnFromParents(cols[i]);
        if(c == 0) continue;
        if(UidWarehouse::instance().related(c, sourceColumn))
        {
            return c;
        }
    }
    return 0;
}

Table *Table::duplicate()
{
    Table* result = new Table();

    result->m_description = m_description;
    result->m_columns = m_columns;
    result->m_indices = m_indices;
    result->m_foreignKeys = m_foreignKeys;
    result->m_startupValues = m_startupValues;
    result->m_parent = m_parent;
    result->m_version = m_version;
    result->m_foreignKeyCommands = m_foreignKeyCommands;
    result->m_tableInstances = m_tableInstances;
    result->m_children = m_children;
    result->m_parentUid = m_parentUid;
    result->m_tempTabName = m_tempTabName;

    return result;
}
