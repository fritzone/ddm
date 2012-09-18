#include "ValueSp.h"
#include "ValueSpInstance.h"
#include "uids.h"

ValueSp::ValueSp(const QString& sqlPrUid,
                         const QString& referringObjectClassUid,
                         const QString &name, const QString& propertyGuiText,
                         const QString& group, const QString& value,
                         int major, int minor, Version *v):
    Sp(sqlPrUid, referringObjectClassUid, name, propertyGuiText, group, major, minor, v),
    m_value(value)
{
}

QUuid ValueSp::getClassUid() const
{
    return QUuid(uidValueSp);
}

SpInstance* ValueSp::instantiate()
{
    ValueSpInstance* vlspi = new ValueSpInstance(this, version());
    vlspi->set(m_value);
    return vlspi;
}

SpInstance* ValueSp::createSpecifiedInstance(const QString& spi_uid)
{
    ValueSpInstance* vlspi = new ValueSpInstance(this, spi_uid, version());
    vlspi->set(m_value);
    return vlspi;
}
