#include "core_ObjectWithUid.h"
#include "UidWarehouse.h"
#include "uids.h"

ObjectWithUid::ObjectWithUid() : m_uid(nullUid), m_sourceUid(nullUid)
{
}

ObjectWithUid::ObjectWithUid(const QString& uid) : m_uid(uid), m_sourceUid(nullUid)
{
}

QStringList ObjectWithUid::sourceUids() const
{
    QStringList thisUpwardsSourceUids;
    thisUpwardsSourceUids.append(getObjectUid());
    const ObjectWithUid* c = this;
    while(true)
    {
        QString srcUid = c->getSourceUid();
        if(srcUid == nullUid) break;

        thisUpwardsSourceUids.append(srcUid);
        c = dynamic_cast<ObjectWithUid*>(UidWarehouse::instance().getElement(srcUid));
        if(!c) break;
    }
    return thisUpwardsSourceUids;
}
