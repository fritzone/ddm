#ifndef CORE_OBJECTWITHUID_H
#define CORE_OBJECTWITHUID_H

#include <QUuid>

class Version;

/**
 * This class is the base class for all the objects that are having UIDs for versioning, parenting and other issues.
 */
class ObjectWithUid
{
public:

    ObjectWithUid();

    ObjectWithUid(const QString&);

    virtual ~ObjectWithUid() {}

    /**
     * Returns the class Uid of the object (from the uids.h file). The objects belonging to a given class share the
     * same class id.
     */
    virtual QUuid getClassUid() const = 0;

    /**
     * Returns the Uid of the object itself. This is unique for all the objects in the solution.
     */
    QUuid getObjectUid() const
    {
        return m_uid;
    }

    void setForcedUid(const QString& uid)
    {
        m_uid = QUuid(uid);
    }

private:

    QUuid m_uid;
};

#endif // CORE_OBJECTWITHUID_H
