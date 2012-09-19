#include "core_ObjectWithUid.h"
#include "UidWarehouse.h"
#include "uids.h"

ObjectWithUid::ObjectWithUid() : m_uid(nullUid)
{
}

ObjectWithUid::ObjectWithUid(const QString& uid, Version* v) : m_uid(uid), m_version(v)
{
    UidWarehouse::instance().addElement(this, v);
}
