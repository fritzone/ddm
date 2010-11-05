#include "Column.h"
#include "UserDataType.h"

#include <QApplication>
#include <QClipboard>

Column::Column(const QString& name, const UserDataType* type, bool pk, bool autoInc) : NamedItem(name), m_type(type), m_pk(pk), m_autoIncrement(autoInc)
{

}

void Column::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement columnElement = doc.createElement("Column");      // will hold the data in this element

    columnElement.setAttribute("Name", m_name);
    columnElement.setAttribute("PK", m_pk);
    columnElement.setAttribute("Type", m_type->getName());
    columnElement.setAttribute("AutoIncrement", m_autoIncrement);

    {
    QDomElement descElement = doc.createElement("Description");  // description
    QDomText descNode = doc.createTextNode(m_description);
    descElement.appendChild(descNode);
    columnElement.appendChild(descElement);
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

void Column::setDescription(const QString& desc)
{
    m_description = desc;
}

QString Column::getDescription() const
{
    return m_description;
}

bool Column::hasAutoIncrement() const
{
    return m_autoIncrement;
}

void Column::setAutoIncrement(bool a)
{
    m_autoIncrement = a;
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
