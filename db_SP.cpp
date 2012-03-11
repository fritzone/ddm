#include "db_SP.h"

#include <QDebug>

Sp::Sp(const QString &sqlRoleUid, const QString &guiClassUid, const QString& referringObjectClassUid, const QString& propertyName, const QString& propertyGuiText) :
    ObjectWithUid(QUuid::createUuid().toString()), NamedItem(propertyName),
    m_sqlRoleUid(sqlRoleUid), m_guiClassUid(guiClassUid), m_referredObjectClasUid(referringObjectClassUid),
    m_propertyGuiText(propertyGuiText)
{
}
