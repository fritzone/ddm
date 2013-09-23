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
    SerializableElement(), CopyableElement(), IssueOriginator(),
    NamedItem(name), ItemWithDescription(), VersionElement(uid, v), ObjectWithSpInstances(),
    TreeItem(),
    m_type(type), m_pk(pk)
{

}

void Column::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement columnElement = doc.createElement("Column");      // will hold the data in this element

    columnElement.setAttribute("Name", m_name);
    columnElement.setAttribute("PK", m_pk);
    columnElement.setAttribute("Type", m_type->getName());
    columnElement.setAttribute("uid", getObjectUid().toString());
    columnElement.setAttribute("class-uid", getClassUid().toString());
    columnElement.setAttribute("source-uid", getSourceUid().toString());

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

CloneableElement* Column::clone(Version* /*sourceVersion*/, Version *targetVersion)
{
    const UserDataType* thisDataType = getDataType();
    if(thisDataType == 0)
    {
        return 0;
    }
    QString thisDataTypeName = thisDataType->getName();
    UserDataType* udt = targetVersion->getDataType(thisDataTypeName);
    Column* result = new Column(QUuid::createUuid().toString(), getName(), udt, isPk(), targetVersion);
    result->setSourceUid(getObjectUid());
    // now fix the SPs of the column
    cloneSps(result);

    return result;
}
