#ifndef CORE_CLONEABLEELEMENT_H
#define CORE_CLONEABLEELEMENT_H

#include <QUuid>

class Version;

/**
 * All the objects that can be cloned (ie: create an identical copy, with a different GUID) must derive from this.
 * This class maintains a link to the "source" via the UID of it, in order for the version comparer to know
 * from where this element came
 */
class CloneableElement
{
public:
    CloneableElement();

    /**
     * Clones this element which is supposed to reside in the
     * sourceVersion to reside in the targetVersion (this is for the uids)
     */
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion) = 0;

    void setSourceUid(const QUuid& o);

    QUuid getSourceUid() const;



private:

    QUuid m_sourceUid;

};

#endif // CORE_CLONEABLEELEMENT_H
