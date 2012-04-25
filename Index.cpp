#include "Index.h"
#include "Column.h"
#include "uids.h"
#include "SpInstance.h"

Index::Index(const QString &name, Table* tab, const QString& uid) :
    NamedItem(name), ObjectWithUid(uid), ObjectWithSpInstances(),
    m_owner(tab), m_columns(), m_columnsWithSpInstances()
{
}

void Index::addColumn(const Column* column, const QString &order)
{
    ColumnAndOrder* cando = new ColumnAndOrder;
    cando->c = column;
    cando->order = order;
    m_columnsWithSpInstances.insert(column, QMap<QString, QVector<SpInstance*> > ());
    m_columns.append(cando);
}


void Index::addColumn(const Column* column, const QString& order, int pos)
{
    ColumnAndOrder* cando = new ColumnAndOrder;
    cando->c = column;
    cando->order = order;

    m_columns.insert(pos - 1, cando);
    m_columnsWithSpInstances.insert(column, QMap<QString, QVector<SpInstance*> > ());
}

bool Index::hasColumn(const Column* column) const
{
    QVector<const Column*> columns = getColumns();
    for(int i=0; i<columns.size(); i++)
    {
        if(columns[i] == column)
        {
            return true;
        }
    }
    return false;
}

void Index::resetColumns()
{
    m_columns.clear();
    m_columnsWithSpInstances.clear();
}

void Index::addSpToColumn(const Column *c, const QString &db, SpInstance *spi)
{
    if(m_columnsWithSpInstances.contains(c))
    {
        QMap<QString, QVector<SpInstance*> >& map1 = m_columnsWithSpInstances[c];
        if(map1.contains(db))
        {
            QVector<SpInstance*>& v1 = map1[db];
            v1.append(spi);
        }
        else
        {
            QVector<SpInstance*> v1;
            v1.append(spi);
            map1.insert(db, v1);
        }
    }
    else
    {
        QMap<QString, QVector<SpInstance*> > map1;
        QVector<SpInstance*> v1;
        v1.append(spi);
        map1.insert(db, v1);
        m_columnsWithSpInstances.insert(c, map1);
    }
}

void Index::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement indexElement = doc.createElement("Index");      // will hold the Index

    indexElement.setAttribute("Name", getName());
    indexElement.setAttribute("uid", getObjectUid());
    indexElement.setAttribute("class-uid", getClassUid().toString());

    {
    QDomElement sqlColumnsElement = doc.createElement("Columns");
    for(int i=0; i<m_columns.size(); i++)
    {
        QDomElement sqlColumnElement = doc.createElement("Column");
        sqlColumnElement.setAttribute("Name", m_columns[i]->c->getName());
        sqlColumnsElement.appendChild(sqlColumnElement);

        QDomElement cspsElement = doc.createElement("ColumnsSps");
        // TODO: Now serialize the SP instances of the given column
        for(int j=0; j<m_columnsWithSpInstances.keys().size(); j++)
        {
            const Column* c = m_columnsWithSpInstances.keys().at(j);
            QMap<QString, QVector<SpInstance*> > map1 = m_columnsWithSpInstances[c];
            for(int k=0; k<map1.keys().size(); k++)
            {
                QString db = map1.keys().at(k);
                QDomElement dbElement = doc.createElement("DbEnginesps");
                dbElement.setAttribute("Name", db);
                QVector<SpInstance*> spis = map1[db];
                for(int l=0; l<spis.size(); l++)
                {
                    spis.at(l)->serialize(doc, dbElement);
                }
                cspsElement.appendChild(dbElement);
            }
            sqlColumnElement.appendChild(cspsElement);
        }
    }
    indexElement.appendChild(sqlColumnsElement);
    }

    // save the sps of the index
    {
    QDomElement spsElement = doc.createElement("SpInstances");
    serialize_spinstances(doc, spsElement);
    indexElement.appendChild(spsElement);
    }

    parent.appendChild(indexElement);
}

QUuid Index::getClassUid() const
{
    return QUuid(uidIndex);
}

QMap<QString, QVector<SpInstance*> > Index::getSpsOfColumn(const Column* c) const
{
    if(m_columnsWithSpInstances.contains(c))
    {
        return m_columnsWithSpInstances[c];
    }

    return QMap<QString, QVector<SpInstance*> >();
}
