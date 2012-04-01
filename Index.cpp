#include "Index.h"
#include "Column.h"
#include "uids.h"

Index::Index(const QString &name, Table* tab, const QString& uid) : NamedItem(name), ObjectWithUid(uid), ObjectWithSpInstances(), m_owner(tab), m_columns()
{
}

void Index::addColumn(const Column* column)
{
    m_columns.append(column);
}

void Index::addColumn(const Column* column, int pos)
{
    m_columns.insert(pos - 1, column);
}

bool Index::hasColumn(const Column* column) const
{
    for(int i=0; i<m_columns.size(); i++)
    {
        if(m_columns[i] == column)
        {
            return true;
        }
    }
    return false;
}

void Index::resetColumns()
{
    m_columns.clear();
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
        sqlColumnElement.setAttribute("Name", m_columns[i]->getName());
        sqlColumnsElement.appendChild(sqlColumnElement);
    }
    indexElement.appendChild(sqlColumnsElement);
    }

    // save the sps
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
