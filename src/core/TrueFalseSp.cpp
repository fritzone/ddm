#include "TrueFalseSp.h"
#include "TrueFalseSpInstance.h"
#include "uids.h"

TrueFalseSp::TrueFalseSp(const QString& sqlPrUid, const QString& referringObjectClassUid, const QString &name, const QString& propertyGuiText, const QString& group, bool defaultValue, int major, int minor, Version* v) :
    Sp(sqlPrUid, referringObjectClassUid, name, propertyGuiText, group, major, minor, v), m_defaultValue(defaultValue)
{
}

QUuid TrueFalseSp::getClassUid() const
{
    return QUuid(uidTrueFalseSp);
}

SpInstance* TrueFalseSp::instantiate()
{
    TrueFalseSpInstance* inst = new TrueFalseSpInstance(this, version());
    if(m_defaultValue)
    {
        inst->set("TRUE");
    }
    else
    {
        inst->set("FALSE");
    }
    return inst;
}

SpInstance* TrueFalseSp::createSpecifiedInstance(const QString& spi_uid)
{
    return new TrueFalseSpInstance(this, spi_uid, version());
}
