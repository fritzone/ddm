#include "core_ForeignKey.h"
#include "core_Index.h"
#include "Version.h"
#include "uids.h"
#include "ddm_strings.h"

ForeignKey::ForeignKey(Version *v, const QString &uid) : TreeItem(), SerializableElement(), NamedItem(), VersionElement(uid, v),
    m_associations(), m_autoGenIndices(), tempautogenIndices(), m_onUpdate(), m_onDelete()
{
}

void ForeignKey::removeAssociation(const QString& fcName, const QString& lcName)
{
    for(int i=0; i<m_associations.size(); i++)
    {
        QString foreignName = m_associations[i]->getForeignColumn()?m_associations[i]->getForeignColumn()->getName():
                                                                   m_associations[i]->getSForeignColumn();
        QString localName = m_associations[i]->getLocalColumn()?m_associations[i]->getLocalColumn()->getName():
                                                                   m_associations[i]->getSLocalColumn();

        if(foreignName == fcName && localName == lcName)
        {
            m_associations.remove(i);
            return;
        }
    }
}


bool ForeignKey::hasAssociation(const QString& fcName, const QString& lcName)
{
    for(int i=0; i<m_associations.size(); i++)
    {
        QString assocForeignName = m_associations[i]->getForeignColumn()?
                    m_associations[i]->getForeignColumn()->getName():
                    m_associations[i]->getSForeignColumn();

        QString assocLocalName = m_associations[i]->getLocalColumn()?
                    m_associations[i]->getLocalColumn()->getName():
                    m_associations[i]->getSLocalColumn();

        if(assocForeignName == fcName && assocLocalName == lcName)
        {
            return true;
        }
    }

    return false;
}

void ForeignKey::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement fkElement = doc.createElement("ForeignKey");      // will hold the foreign keys' stuff

    fkElement.setAttribute("Name", m_name);
    fkElement.setAttribute("OnUpdate", m_onUpdate);
    fkElement.setAttribute("OnDelete", m_onDelete);
    fkElement.setAttribute("uid", getObjectUid().toString());
    fkElement.setAttribute("class-uid", getClassUid().toString());
    fkElement.setAttribute("source-uid", getSourceUid().toString().length() ? getSourceUid().toString() : strNA);

    // save the associations
    QDomElement associationsElement = doc.createElement("Associations");
    for(int i=0; i<m_associations.size(); i++)
    {
        QString foreignTabName = m_associations[i]->getForeignTable()->getName();
        QString localTabName = m_associations[i]->getLocalTable()->getName();
        QString foreignColumn = m_associations[i]->getForeignColumn()->getName();
        QString localColumn = m_associations[i]->getLocalColumn()->getName();
        QDomElement assocElement = doc.createElement("Association");      // will hold the data in this element
        assocElement.setAttribute("ForeignTable", foreignTabName);
        assocElement.setAttribute("LocalTable", localTabName);
        assocElement.setAttribute("ForeignColumn", foreignColumn);
        assocElement.setAttribute("LocalColumn", localColumn);
        associationsElement.appendChild(assocElement);
    }

    QDomElement autoGenIndices = doc.createElement("AutoGenIndices");

    for(int i=0; i<m_autoGenIndices.size(); i++)
    {
        QDomElement autoGenElement = doc.createElement("Index");      // will hold the data in this element
        autoGenElement.setAttribute("Table", m_autoGenIndices.at(i)->getTable()->getName());
        autoGenElement.setAttribute("Name", m_autoGenIndices.at(i)->getName());
        autoGenIndices.appendChild(autoGenElement);
    }

    fkElement.appendChild(associationsElement);
    fkElement.appendChild(autoGenIndices);
    parent.appendChild(fkElement);
}

const Table* ForeignKey::getForeignTable() const
{
    for(int i=0; i<m_associations.size(); i++)
    {
        return m_associations[i]->getForeignTable();
    }
    return 0;
}

const Table* ForeignKey::getLocalTable() const
{
    for(int i=0; i<m_associations.size(); i++)
    {
        return m_associations[i]->getLocalTable();
    }
    return 0;
}

QString ForeignKey::getForeignTableName() const
{
    QString result = "";
    for(int i=0; i<m_associations.size(); i++)
    {
        result = m_associations[i]->getSForeignTable();
        break;
    }
    return result;
}

QString ForeignKey::getDescriptiveText() const
{
    QString result = "";
    QString localTable = "";
    QString foreignTable = "";
    for(int i=0; i<m_associations.size(); i++)
    {
        localTable = m_associations[i]->getSLocalTable();
        foreignTable = m_associations[i]->getSForeignTable();
        break;
    }
    localTable += "(";
    foreignTable += "(";
    for(int i=0; i<m_associations.size(); i++)
    {
        localTable += m_associations[i]->getSLocalColumn();
        foreignTable += m_associations[i]->getSForeignColumn();

        if(i<m_associations.size() - 1)
        {
            localTable += ", ";
            foreignTable += ", ";
        }
    }
    localTable += ") -> ";
    foreignTable += ")";
    result = localTable + foreignTable;
    return result;
}

QString ForeignKey::getLocalDescriptiveText() const
{
    QString result = "";
    for(int i=0; i<m_associations.size(); i++)
    {
        result += m_associations[i]->getSLocalColumn();
        if(i<m_associations.size() - 1)
        {
            result += ", ";
        }
    }

    return result;
}

QString ForeignKey::getForeignDescriptiveText() const
{
    QString result = "";
    for(int i=0; i<m_associations.size(); i++)
    {
        result += m_associations[i]->getSForeignColumn();
        if(i<m_associations.size() - 1)
        {
            result += ", ";
        }
    }
    return result;
}

QVector<const Column*> ForeignKey::foreignColumns() const
{
    QVector<const Column*> result;
    for(int i=0; i<m_associations.size(); i++)
    {
        result.append(m_associations.at(i)->getForeignColumn());
    }
    return result;
}

QVector<const Column*> ForeignKey::localColumns() const
{
    QVector<const Column*> result;
    for(int i=0; i<m_associations.size(); i++)
    {
        result.append(m_associations.at(i)->getLocalColumn());
    }
    return result;
}

void ForeignKey::onDelete()
{
    for(int i=0; i<m_autoGenIndices.size(); i++)
    {
        Table* t = m_autoGenIndices.at(i)->getTable();
        if(!t)
        {
            continue;
        }

        t->removeIndex(m_autoGenIndices.at(i));
    }
}

void ForeignKey::finalizeAutogeneratedIndices(Version* v)
{
    m_autoGenIndices.clear();
    for(int i=0; i<tempautogenIndices.size(); i++)
    {
        Table* t = v->getTable(tempautogenIndices.at(i).tabName);
        if(t)
        {
            Index* idx = t->getIndex(tempautogenIndices.at(i).indexName);
            if(idx)
            {
                m_autoGenIndices.append(idx);
            }
        }
    }
    tempautogenIndices.clear();
}

QUuid ForeignKey::getClassUid() const
{
    return uidForeignKey;
}
