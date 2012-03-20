#include "TrueFalseSp.h"
#include "TrueFalseSpInstance.h"
#include "uids.h"

TrueFalseSp::TrueFalseSp(const QString& sqlPrUid, const QString& referringObjectClassUid, const QString &name, const QString& propertyGuiText, const QString& group) :
    Sp(sqlPrUid, referringObjectClassUid, name, propertyGuiText, group)
{
}

QUuid TrueFalseSp::getClassUid() const
{
    return QUuid(uidTrueFalseSp);
}

SpInstance* TrueFalseSp::instantiate()
{
    return new TrueFalseSpInstance(this);
}

SpInstance* TrueFalseSp::createSpecifiedInstance(const QString& spi_uid)
{
    return new TrueFalseSpInstance(this, spi_uid);
}
