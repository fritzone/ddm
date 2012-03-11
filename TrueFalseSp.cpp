#include "TrueFalseSp.h"
#include "uids.h"

TrueFalseSp::TrueFalseSp(const QString& sqlPrUid, const QString& dbObjectUid, const QString& referringObjectClassUid, const QString &name, const QString& propertyGuiText) :
    Sp(sqlPrUid, dbObjectUid, referringObjectClassUid, name, propertyGuiText)
{}

QUuid TrueFalseSp::getClassUid() const
{
    return QUuid(uidTrueFalseObj);
}

SpInstance* TrueFalseSp::instantiate()
{
    return 0;
}
