#include "db_SP.h"

Sp::Sp(const QString &sqlRoleUid, const QString& referringObjectClassUid, const QString& propertyName, const QString& propertyGuiText) :
    ObjectWithUid(QUuid::createUuid().toString()), NamedItem(propertyName),
    m_sqlRoleUid(sqlRoleUid), m_referredObjectClasUid(referringObjectClassUid),
    m_propertyGuiText(propertyGuiText)
{
}

void Sp::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement spElement = doc.createElement("Sp");
    spElement.setAttribute("Name", getName());
    spElement.setAttribute("uid", getObjectUid());
    spElement.setAttribute("class-uid", getClassUid());
    spElement.setAttribute("sql-role-uid", getSqlRoleUid());
    spElement.setAttribute("referred-object-class-uid", getReferredObjectClassUid());
    parent.appendChild(spElement);
}
