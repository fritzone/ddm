#include "db_SP.h"

#include <QDebug>

Sp::Sp(const QString &sqlPropertyUid, const QString &dbObjectUid, const QString& propertyName, const QString& propertyGuiText) :
    ObjectWithUid(QUuid::createUuid().toString()), NamedItem(propertyName),
    m_sqlPropertyUid(sqlPropertyUid), m_destinationUid(dbObjectUid), m_propertyGuiText(propertyGuiText)
{
}
