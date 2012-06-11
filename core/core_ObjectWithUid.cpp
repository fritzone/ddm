#include "core_ObjectWithUid.h"
#include "UidWarehouse.h"
#include "uids.h"

ObjectWithUid::ObjectWithUid() : m_uid(nullUid)
{
}

ObjectWithUid::ObjectWithUid(const QString& uid) : m_uid(uid)
{
    UidWarehouse::instance().addElement(this);
}
