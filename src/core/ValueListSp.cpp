#include "ValueListSp.h"
#include "ValueListSpInstance.h"
#include "uids.h"

ValueListSp::ValueListSp(const QString& sqlPrUid,
                         const QString& referringObjectClassUid,
                         const QString &name, const QString& propertyGuiText,
                         const QString& group, const QStringList& values,
                         int defaultValueIndex, int major, int minor, Version* v):
    Sp(sqlPrUid, referringObjectClassUid, name, propertyGuiText, group, major, minor, v),
    m_values(values),
    m_defaultValuesIndex(defaultValueIndex<values.length()?defaultValueIndex:0)
{
}

QUuid ValueListSp::getClassUid() const
{
    return QUuid(uidValueListSp);
}

SpInstance* ValueListSp::instantiate()
{
    ValueListSpInstance* vlspi = new ValueListSpInstance(this, version());
    vlspi->set(m_values[m_defaultValuesIndex]);
    return vlspi;
}

SpInstance* ValueListSp::createSpecifiedInstance(const QString& spi_uid)
{
    ValueListSpInstance* vlspi = new ValueListSpInstance(this, spi_uid, version());
    vlspi->set(m_values[m_defaultValuesIndex]);
    return vlspi;
}
