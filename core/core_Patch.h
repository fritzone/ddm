#ifndef CORE_PATCH_H
#define CORE_PATCH_H

#include <QVector>
#include <QStringList>
#include <QMap>

#include "NamedItem.h"
#include "TreeItem.h"
#include "SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "ActionDeleteTable.h"

class Version;

class Patch : public NamedItem, public TreeItem, public SerializableElement, public ObjectWithUid
{
public:

    /**
     * @brief Patch - create a new Patch object
     * @param v - the version for which the patch is requested
     */
    Patch(Version* v);

    /**
     * @brief addElement adds a locked element to the patch. Creates a local copy of it.
     * We get here from the unlocking of an element
     * @param uid - the uid which will be addedd to the patch
     */
    void addElement(const QString& uid);

    /**
     * @brief addNewElement adds a NEW element to the patch. This should always be called after addElement.
     * We get here when adding a new table in a locked version
     * @param uid
     */
    void addNewElement(const QString& uid);

    /**
     * @brief removeElement removes the element from the patch. Erases
     * from the internal structures and also puts it back to the version!
     * @param uid
     */
    void removeElement(const QString& uid);

    /**
     * @brief markElemetForDeletion marks the element as being deleted from the Version
     * @param uid the UID of the element
     */
    void markElementForDeletion(const QString& uid);

    /**
     * @brief addDeletedTable
     * @param uid
     * @param td
     */
    void addDeletedTable(const QString& uid, TableDeletionAction* td);

    /**
     * @brief getTDA
     * @param uid
     * @return
     */
    TableDeletionAction* getTDA(const QString& uid);

    /**
     * @brief undeleteObject Undeletes the object with the given UID
     * @param uid
     */
    void undeleteObject(const QString& uid);

    /**
     * @brief getDeletedObject returns a delete object, 0 if not found
     * @param uid
     * @return
     */
    ObjectWithUid* getDeletedObject(const QString& uid);

    /**
     * @brief serialize
     * @param doc
     * @param parent
     * serialize this patch to be saved.
     */
    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    /**
     * @brief getClassUid returns the patch class Uid
     * @return the patch class Uid
     */
    virtual QUuid getClassUid() const;


private:
    // the UIDS that are locked in this change
    QStringList m_lockedUids;

    // the original elements, in case we need to re-lock them, we just copy them back and fix the sourceUid
    // they are stored as serialized stuff, and upon serialization they are stored as CDATA
    QMap<QString, QString> m_originals;

    // the UIDs of the elements that were created in this patch
    QStringList m_newUids;

    // the UIDs that were deleted from teh version in case of revert
    QStringList m_deletedUids;

    // the delete objects will be placed in here
    QMap<QString, ObjectWithUid*> m_deletedObjects;

    // if this patch was suspended by the user (ie: it
    bool m_suspended;

    QMap<QString, TableDeletionAction*> m_tableDeletions;
};

#endif // CORE_PATCH_H
