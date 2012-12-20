#include "core_ObjectWithUid.h"
#include "UidWarehouse.h"
#include "uids.h"

ObjectWithUid::ObjectWithUid() : m_version(0), m_uid(nullUid), m_sourceUid(nullUid)
{
}

ObjectWithUid::ObjectWithUid(const QString& uid, Version* v) : m_version(v), m_uid(uid), m_sourceUid(nullUid)
{
    UidWarehouse::instance().addElement(this, v);
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
