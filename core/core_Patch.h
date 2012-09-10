#ifndef CORE_PATCH_H
#define CORE_PATCH_H

#include <QVector>
#include <QStringList>
#include <QMap>

class Version;

class Patch
{
public:

    /**
     * @brief Patch - create a new Patch object
     * @param v - the version for which the patch is requested
     */
    Patch(Version* v);

    /**
     * @brief addElement adds an element to the patch. Creates a local copy of it.
     * We get here from the unlocking of an element
     * @param uid - the uid which will be addedd to the patch
     */
    void addElement(const QString& uid);

    /**
     * @brief removeElement removes the element from the patch. Erases
     * from the internal structures and also puts it back to the version!
     * @param uid
     */
    void removeElement(const QString& uid);

private:
    // the UIDS that are locked in this change
    QStringList m_lockedUids;

    // the original elements, in case we need to re-lock them, we just copy them back and fix the sourceUid
    // they are stored as serialized stuff
    QMap<QString, QString> m_originals;

    // the version in which this patch is active
    Version* m_version;
};

#endif // CORE_PATCH_H
