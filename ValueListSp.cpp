#include "ValueListSp.h"
#include "ValueListSpInstance.h"
#include "uids.h"

ValueListSp::ValueListSp(const QString& sqlPrUid, const QString& referringObjectClassUid, const QString &name, const QString& propertyGuiText, const QString& group, const QStringList& values):
    Sp(sqlPrUid, referringObjectClassUid, name, propertyGuiText, group), m_values(values)
{
}

QUuid ValueListSp::getClassUid() const
{
    return QUuid(uidValueListSp);
}

SpInstance* ValueListSp::instantiate()
{
    return new ValueListSpInstance(this);
}

SpInstance* ValueListSp::createSpecifiedInstance(const QString& spi_uid)
{
    return new ValueListSpInstance(this, spi_uid);
}
