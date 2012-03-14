#include "TrueFalseSp.h"
#include "TrueFalseSpInstance.h"
#include "uids.h"

TrueFalseSp::TrueFalseSp(const QString& sqlPrUid, const QString& referringObjectClassUid, const QString &name, const QString& propertyGuiText) :
    Sp(sqlPrUid, referringObjectClassUid, name, propertyGuiText)
{}

QUuid TrueFalseSp::getClassUid() const
{
    return QUuid(uidTrueFalseObj);
}

SpInstance* TrueFalseSp::instantiate()
{
    return new TrueFalseSpInstance(this);
}

SpInstance* TrueFalseSp::createSpecifiedInstance(const QString& spi_uid)
{
    TrueFalseSpInstance* spi = new TrueFalseSpInstance(this, spi_uid);
    return spi;
}
