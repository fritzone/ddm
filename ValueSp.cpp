#include "ValueSp.h"
#include "ValueSpInstance.h"
#include "uids.h"

ValueSp::ValueSp(const QString& sqlPrUid,
                         const QString& referringObjectClassUid,
                         const QString &name, const QString& propertyGuiText,
                         const QString& group, const QString& value,
                         int major, int minor):
    Sp(sqlPrUid, referringObjectClassUid, name, propertyGuiText, group, major, minor),
    m_value(value)
{
}

QUuid ValueSp::getClassUid() const
{
    return QUuid(uidValueSp);
}

SpInstance* ValueSp::instantiate()
{
    ValueSpInstance* vlspi = new ValueSpInstance(this);
    vlspi->set(m_value);
    return vlspi;
}

SpInstance* ValueSp::createSpecifiedInstance(const QString& spi_uid)
{
    ValueSpInstance* vlspi = new ValueSpInstance(this, spi_uid);
    vlspi->set(m_value);
    return vlspi;
}
