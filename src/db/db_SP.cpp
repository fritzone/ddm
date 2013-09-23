#include "db_SP.h"

Sp::Sp(const QString &sqlRoleUid, const QString& referringObjectClassUid, const QString& propertyName, const QString& propertyGuiText, const QString& group, int majorV, int minorV, Version* v) :
    VersionElement(QUuid::createUuid().toString(), v), NamedItem(propertyName),
    m_sqlRoleUid(sqlRoleUid), m_referredObjectClasUid(referringObjectClassUid),
    m_propertyGuiText(propertyGuiText), m_group(group), m_supportedMajorVersion(majorV),
    m_supportedMinorVersion(minorV)
{
}

void Sp::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement spElement = doc.createElement("Sp");
    spElement.setAttribute("Name", getName());
    spElement.setAttribute("uid", getObjectUid().toString());
    spElement.setAttribute("class-uid", getClassUid().toString());
    spElement.setAttribute("sql-role-uid", getSqlRoleUid());
    spElement.setAttribute("referred-object-class-uid", getReferredObjectClassUid().toString());
    parent.appendChild(spElement);
}
