#include "Table.h"
#include "Column.h"
#include "Index.h"
#include "Version.h"
#include "ForeignKey.h"
#include "DatabaseEngine.h"
#include "Configuration.h"
#include "UserDataType.h"
#include "AbstractStorageEngine.h"
#include "AbstractSQLGenerator.h"
#include "DraggableGraphicsItem.h"
#include "IconFactory.h"
#include "AbstractIndextypeProvider.h"

#include <QPen>

Table::Table(Version* v) :  m_name(""), m_description(""), m_columns(), m_indices(), m_foreignKeys(), m_startupValues(),
                            m_parent(0), m_persistent(false), m_temporary(false), m_storageEngine(0), m_diagramEntity(0), m_version(v)
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

ForeignKey* Table::getForeignKeyToTable(const QString& tableName) const
{
    for(int i=0; i<m_foreignKeys.size(); i++)
    {
        if(m_foreignKeys[i]->getForeignTableName() == tableName)
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


void Table::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableElement = doc.createElement("Table");      // will hold the data in this element
    tableElement.setAttribute("Name", m_name);
    tableElement.setAttribute("Persistent", m_persistent);
    tableElement.setAttribute("Temporary", m_temporary);
    tableElement.setAttribute("Parent",m_parent?m_parent->getName():"N/A");
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

void Table::prepareDiagramEntity()
{
    DraggableGraphicsViewItem* grp = new DraggableGraphicsViewItem(this);

    QGraphicsTextItem* txtName = new QGraphicsTextItem(getName(), grp);
    txtName->setPos(0,0);
    txtName->setToolTip(m_name);
    int py = txtName->boundingRect().height();
    int headerHeight = py;
    int maxX = txtName->boundingRect().right();
    QStringList fullC = fullColumns();
    for(int i=0; i<fullC.size(); i++)
    {
        Column* col = getColumn(fullC[i]);
        if(!col)
        {
            col = getColumnFromParents(fullC[i]);
            if(!col)
            {
                return; // Shouldn't ever happen
            }
        }


        QGraphicsTextItem* txtColName = new QGraphicsTextItem(fullC[i], grp);
        txtColName->setY(py);

        QGraphicsPixmapItem* icon = new QGraphicsPixmapItem(col->getDataType()->getIcon().pixmap(16,16), grp);
        if(col->isPk())
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

    for(int i=0; i<fullC.size(); i++)
    {
        Column* col = getColumn(fullC[i]);
        if(!col)
        {
            col = getColumnFromParents(fullC[i]);
            if(!col)
            {
                return; // Shouldn't ever happen
            }
        }

        QGraphicsTextItem* txtColName = new QGraphicsTextItem(col->getDataType()->sqlAsString(), grp);
        txtColName->setY(py);
        txtColName->setX(maxX);
        py += txtColName->boundingRect().height();
        finMaxX = txtColName->boundingRect().width() + maxX > finMaxX?txtColName->boundingRect().width() + maxX:finMaxX;
    }

    maxX = finMaxX;

    QGraphicsRectItem* rect = new QGraphicsRectItem(0,0, maxX, py, grp);
    QPen tPen;

    if(m_temporary)
    {
        tPen.setStyle(Qt::DashLine);
    }

    if(m_persistent)
    {
        tPen.setWidth(2);
    }
    if(Configuration::instance().drawTableTypes()) rect->setPen(tPen);

    grp->setLastX(maxX);
    grp->setLastY(py);
    QGraphicsRectItem* recta = new QGraphicsRectItem(0,0, maxX, headerHeight, grp);
    if(Configuration::instance().drawTableTypes()) recta->setPen(tPen);
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

QStringList Table::generateSqlSource(AbstractSqlGenerator *generator, QHash<QString,QString> opts)
{
    const_cast<Table*>(this)->restartSqlRendering();
    return generator->generateCreateTableSql(const_cast<Table*>(this), opts, getName());
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

bool Table::createAutoIndex(QVector<const Column*> cols)
{
    QString idxName = getAvailableIndexName("autoidx");
    Index* idx = new Index(idxName, m_version->getDatabaseEngine()->getIndextypeProvider()->getDefaultIndextype());
    for(int i=0; i<cols.size(); i++)
    {
        idx->addColumn(cols.at(i));
    }
    addIndex(idx);
    return true;
}
