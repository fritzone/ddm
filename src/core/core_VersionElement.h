#ifndef CORE_VERSIONELEMENT_H
#define CORE_VERSIONELEMENT_H

#include "core_ObjectWithUid.h"
#include "../UidWarehouse.h"

class Version;

class VersionElement : public ObjectWithUid
{
public:
    virtual QUuid getClassUid() const = 0;

public:

    VersionElement(const QString& uid, Version *v) : ObjectWithUid(uid), m_version(v)
    {
        // it is ok to cast this to an ObjectWithUid object, the base was already constructed
        UidWarehouse::instance().addElement(dynamic_cast<ObjectWithUid*>(this), v);
    }

    /**
     * @brief Returns the version of this object
     * @return the Version of this object
     */
    Version* version() const
    {
        return m_version;
    }

    void setForcedVersion(Version *v)
    {
        m_version = v;
    }

protected:
    // the version in which this element resides
    Version *m_version;

};

#endif // CORE_VERSIONELEMENT_H
