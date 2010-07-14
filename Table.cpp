#include "Table.h"
#include "Column.h"
#include "Index.h"
#include "ForeignKey.h"
#include "UserDataType.h"
#include "AbstractStorageEngine.h"
#include "DraggableGraphicsItem.h"
#include "IconFactory.h"

Table::Table() : m_name(""), m_description(""), m_columns(), m_indices(), m_foreignKeys(), m_startupValues(), m_parent(0), m_persistent(false), m_storageEngine(0), m_diagramEntity(0)
{
}

void Table::addColumn(Column *column)
{
    m_columns.append(column);
    prepareDiagramEntity();
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
        prepareDiagramEntity();
    }
}

void Table::moveColumnUp(int c)
{
    if(c > 0)
    {
        Column* tc = m_columns[c];
        m_columns[c] = m_columns[c-1];
        m_columns[c-1] = tc;
        prepareDiagramEntity();
    }
}

bool Table::hasIndex(const QString& indexName) const
{
    for(int i=0; i<m_indices.size(); i++)
    {
        if(m_indices[i]->getName() == indexName)
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
        if(m_columns[i]->getName() == colName)
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
        if(m_foreignKeys[i]->getName() == name)
        {
            return m_foreignKeys[i];
        }
    }
    return 0;
}

ForeignKey* Table::getForeignKeyToTable(const QString& tableName)
{
    for(int i=0; i<m_foreignKeys.size(); i++)
    {
        if(m_foreignKeys[i]->getForeignTable() == tableName)
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
        if(m_columns[i]->getName() == name)
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
        prepareDiagramEntity();
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



void Table::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableElement = doc.createElement("Table");      // will hold the data in this element
    tableElement.setAttribute("Name", m_name);
    tableElement.setAttribute("Persistent", m_persistent);
    tableElement.setAttribute("Temporary", m_temporary);
    tableElement.setAttribute("StorageEngine", m_storageEngine?m_storageEngine->name():"");

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

    parent.appendChild(tableElement);
}

void Table::setDefaultValues(QVector <QVector <QString> > & values)
{
    m_startupValues = values;
}

void Table::prepareDiagramEntity()
{
    DraggableGraphicsViewItem* grp = new DraggableGraphicsViewItem(this);

    QGraphicsTextItem* txtName = new QGraphicsTextItem(getName(), grp);
    txtName->setPos(0,0);
    txtName->setToolTip(m_name);
    int py = txtName->boundingRect().height();
    int headerHeight = py;
    int maxX = txtName->boundingRect().right();

    for(int i=0; i<m_columns.size(); i++)
    {
        QGraphicsTextItem* txtColName = new QGraphicsTextItem(m_columns[i]->getName(), grp);
        txtColName->setY(py);

        QGraphicsPixmapItem* icon = new QGraphicsPixmapItem(m_columns[i]->getDataType()->getIcon().pixmap(16,16), grp);
        if(m_columns[i]->isPk())
        {
            QGraphicsPixmapItem* keyicon = new QGraphicsPixmapItem(IconFactory::getKeyIcon().pixmap(16,16), grp);
            keyicon->setY(py + 2);
            keyicon->setX(2);
        }

        txtColName->setX(36);
        icon->setY(py + 2);
        icon->setX(20);
        py+=txtColName->boundingRect().height();
        maxX = txtColName->boundingRect().right() + 20 > maxX ? txtColName->boundingRect().right() + 20:maxX;
    }

    maxX += 30;
    int finMaxX = maxX;
    py = headerHeight;

    for(int i=0; i<m_columns.size(); i++)
    {
        QGraphicsTextItem* txtColName = new QGraphicsTextItem(m_columns[i]->getDataType()->sqlAsString(), grp);
        txtColName->setY(py);
        txtColName->setX(maxX);
        py += txtColName->boundingRect().height();
        finMaxX = txtColName->boundingRect().width() + maxX > finMaxX?txtColName->boundingRect().width() + maxX:finMaxX;
    }

    maxX = finMaxX;

    QGraphicsRectItem* rect = new QGraphicsRectItem(0,0, maxX, py, grp);
    grp->setLastX(maxX);
    grp->setLastY(py);
    QGraphicsRectItem* recta = new QGraphicsRectItem(0,0, maxX, headerHeight, grp);
    QBrush brush (Qt::lightGray);
    QBrush wbrush (Qt::white);
    recta->setBrush(brush);
    rect->setBrush(wbrush);
    rect->setZValue(-1);
    recta->setZValue(0.5);
    txtName->setZValue(1);
    grp->setToolTip(m_description);

    m_diagramEntity = grp;

}
