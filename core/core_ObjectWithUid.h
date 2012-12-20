#ifndef CORE_OBJECTWITHUID_H
#define CORE_OBJECTWITHUID_H

#include <QUuid>

class Version;
class UidWarehouse;

/**
 * This class is the base class for all the objects that are having UIDs for versioning, parenting and other issues.
 * Tracks the UID of the object and also the parent uid (in case of cloning)
 */
class ObjectWithUid
{
public:

    /**
     * Returns the class Uid of the object (from the uids.h file). The objects belonging to a given class share the
     * same class id.
     */
    virtual QUuid getClassUid() const = 0;

public:

    /**
     * Default Constructor
     */
    ObjectWithUid();

    /**
     * @brief Creates a new object with the uid for the given version
     * @param uid
     * @param v
     */
    ObjectWithUid(const QString& uid, Version *v);

    /**
     * Destructor
     */
    virtual ~ObjectWithUid() {}

    /**
     * Returns the Uid of the object itself. This is unique for all the objects in the solution.
     */
    QUuid getObjectUid() const
    {
        return m_uid;
    }

    /**
     * @brief Sets the uid of this object to be the given one
     * @param uid
     */
    void setForcedUid(const QString& uid)
    {
        m_uid = QUuid(uid);
    }

    /**
     * @brief Returns the version of this object
     * @return the Version of this object
     */
    Version* version() const
    {
        return m_version;
    }

    /**
     * @brief sets the SourceUid ofthe object
     * @param o - the UID
     */
    void setSourceUid(const QUuid& o)
    {
        m_sourceUid = o;
    }

    /**
     * @brief get the SourceUid of the object
     * @return the SourceUid of the object
     */
    QUuid getSourceUid() const
    {
        return m_sourceUid;
    }

private:

    friend class UidWarehouse;

    /**
     * @brief return a list of object UIDS that have "contributed" from the first
     * version of the object to the creation ofthis object (ie. all the elements
     * from which this was cloned.
     * @return the list of object UIDs
     */
    QStringList sourceUids() const;

protected:
    // the version in which this element resides
    Version *m_version;

private:

    // the uid of this object
    QUuid m_uid;

    // the source uid of this object (ie: the uid of the element from which this was cloned)
    QUuid m_sourceUid;

};

#endif // CORE_OBJECTWITHUID_H
