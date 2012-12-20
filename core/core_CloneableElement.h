#ifndef CORE_CLONEABLEELEMENT_H
#define CORE_CLONEABLEELEMENT_H

class Version;

/**
 * All the objects that can be cloned (ie: create an identical copy, with a different GUID) must derive from this.
 * This class maintains a link to the "source" via the UID of it, in order for the version comparer to know
 * from where this element came
 */
class CloneableElement
{
public:

    /**
     * Clones this element which is supposed to reside in the
     * sourceVersion to reside in the targetVersion (this is for the uids)
     * @param sourceVersion - the version in which this element resides
     * @param targetVersion - the version in which the element will reside
     */
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion) = 0;

public:

    /**
     * Destructor
     */
    virtual ~CloneableElement() {}

};

#endif // CORE_CLONEABLEELEMENT_H
