#include "core_Column.h"
#include "core_UserDataType.h"
#include "core_Table.h"
#include "uids.h"
#include "Workspace.h"
#include "Version.h"
#include "UidWarehouse.h"

#include <QApplication>
#include <QClipboard>

Column::Column(const QString& uid, const QString& name, const UserDataType* type, bool pk, Version* v) :
    TreeItem(), SerializableElement(), CopyableElement(), IssueOriginator(),
    NamedItem(name), ItemWithDescription(), ObjectWithUid(uid, v), ObjectWithSpInstances(),
    m_type(type), m_pk(pk)
{

}

void Column::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement columnElement = doc.createElement("Column");      // will hold the data in this element

    columnElement.setAttribute("Name", m_name);
    columnElement.setAttribute("PK", m_pk);
    columnElement.setAttribute("Type", m_type->getName());
    columnElement.setAttribute("uid", getObjectUid());
    columnElement.setAttribute("class-uid", getClassUid());

    {
    QDomElement descElement = doc.createElement("Description");  // description
    QDomText descNode = doc.createTextNode(getDescription());
    descElement.appendChild(descNode);
    columnElement.appendChild(descElement);
    }

    // save the sps
    {
    QDomElement spsElement = doc.createElement("SpInstances");
    serialize_spinstances(doc, spsElement);
    columnElement.appendChild(spsElement);
    }

    parent.appendChild(columnElement);

}

const UserDataType* Column::getDataType() const
{
    return m_type;
}

bool Column::isPk() const
{
    return m_pk;
}

void Column::setDataType(const UserDataType* dt)
{
    m_type = dt;
}

void Column::setPk(bool pk)
{
    m_pk = pk;
}

void Column::copy()
{
    QDomDocument doc("ClipboardData");
    QDomElement root = doc.createElement("CopiedColumn");
    serialize(doc, root);
    doc.appendChild(root);
    QString text = doc.toString();

    QApplication::clipboard()->setText(text);
}

QString Column::getFullLocation() const
{
    return m_table->getName() + "." + getName();
}

QUuid Column::getClassUid() const
{
    return uidColumn;
}

CloneableElement* Column::clone(Version *sourceVersion, Version *targetVersion)
{
    UserDataType* udt = targetVersion->getDataType(getDataType()->getName());
    Column* result = new Column(QUuid::createUuid().toString(), getName(), udt, isPk(), targetVersion);
    result->setSourceUid(getObjectUid());
    // now fix the SPs of the column
    cloneSps(result);

    return result;
}

// this is used only in the comparison, the other is supposed to have source ID
// columns are equal if: they have the same name, same SQL type, same primary keyness (for now)
int Column::checkEquality(const Column *other)
{

    if(getObjectUid() == other->getObjectUid()) return 0;

    QString otherUid = other->getObjectUid();
    Q_UNUSED(otherUid);

    // now see if they have any common source uid ...
    QStringList thisUpwardsSourceUids = sourceUids();
    qDebug() << thisUpwardsSourceUids;
    QStringList otherUpwardsSourceUids = other->sourceUids();
    qDebug() << otherUpwardsSourceUids;

    for(int i=0; i<thisUpwardsSourceUids.size(); i++)
    {
        if(otherUpwardsSourceUids.indexOf(thisUpwardsSourceUids[i]) != -1)
        {
            // they are related
            // but if the names are not equal, they are not equal
            if(getName() != other->getName()) return 3;

            // or the PK?
            if(isPk() != other->isPk()) return 2;

            // or the data type?
            if(getDataType()->sqlAsString() != other->getDataType()->sqlAsString()) return 4;

            return 0;
        }
    }
    for(int i=0; i<otherUpwardsSourceUids.size(); i++)
    {
        if(thisUpwardsSourceUids.indexOf(otherUpwardsSourceUids[i]) != -1)
        {   // they are related
            // names are not equal, they are not equal
            if(getName() != other->getName()) return 3;
            if(isPk() != other->isPk()) return 2;
            if(getDataType()->sqlAsString() != other->getDataType()->sqlAsString()) return 4;

            return 0;
        }
    }

    return -1;
}

QStringList Column::sourceUids() const
{
    QStringList thisUpwardsSourceUids;
    thisUpwardsSourceUids.append(getObjectUid());
    const Column* c = this;
    while(true)
    {
        QString srcUid = c->getSourceUid();
        if(srcUid == nullUid) break;

        thisUpwardsSourceUids.append(srcUid);
        c = dynamic_cast<Column*>(UidWarehouse::instance().getElement(srcUid));
        if(!c) break;
    }
    return thisUpwardsSourceUids;
}
