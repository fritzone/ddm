#ifndef CORE_PATCH_H
#define CORE_PATCH_H

#include <QVector>
#include <QStringList>
#include <QMap>

#include "core_NamedItem.h"
#include "TreeItem.h"
#include "core_SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "core_VersionElement.h"
#include "core_Function.h"
#include "core_Procedure.h"
#include "core_View.h"
#include "core_Diagram.h"
#include "core_Diagram.h"
#include "ActionDeleteTable.h"

class Version;

class Patch :
        public NamedItem,
        public SerializableElement,
        public VersionElement,
        public TreeItem
{
public:

    /**
     * @brief Patch - create a new Patch object
     * @param v - the version for which the patch is requested
     */
    Patch(Version* v, bool init);

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
     * @brief addDeletedDiagram
     * @param uid
     * @param td
     */
    template<class T>
    void addDeletedObject(const QString& uid, DeletionAction<T> *dda)
    {
        m_deletions[UidWarehouse::instance().getElement(uid)->getClassUid().toString()][uid] = dda;
    }

    /**
     * @brief getTDA
     * @param uid
     * @return
     */
    TableDeletionAction* getTDA(const QString& uid);

    template<class T>
    DeletionAction<T>* getDeletionAction(const QString &uid)
    {
        ObjectWithUid* ouid = UidWarehouse::instance().getElement(uid);
        QString classUid = ouid->getClassUid().toString();
        if(!m_deletions.contains(classUid))
        {
            return 0;
        }

        if(!m_deletions[classUid].contains(uid))
        {
            return 0;
        }

        DeletionAction<T>* da = static_cast<DeletionAction<T>*> (m_deletions[classUid][uid]);
        return da;

    }

    /**
     * @brief removeTDA
     * @param uid
     */
    void removeTDA(const QString &uid);

    /**
     * @brief undeleteObject Undeletes the object with the given UID
     * @param uid
     */
    bool undeleteObject(const QString& uid);

    /**
     * @brief getDeletedObject returns a delete object, 0 if not found
     * @param uid
     * @return
     */
    ObjectWithUid* getDeletedObject(const QString& uid);

    /**
     * @brief elementWasNewInThisPatch
     * @param uid
     * @return
     */
    bool elementWasNewInThisPatch(const QString& uid);
    bool elementWasLockedInThisPatch(const QString& uid);

    /**
     * @brief removeNewElementBecauseOfDeletion
     * @param uid
     * @return
     */
    bool removeNewElementBecauseOfDeletion(const QString& uid);

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

    bool isSuspended() const
    {
        return m_suspended;
    }

    void setSuspended(bool s)
    {
        m_suspended = s;
    }

    void setLockedUids(const QStringList& luids)
    {
        m_lockedUids = luids;
    }

    void setDeletedUids(const QStringList& duids)
    {
        m_deletedUids = duids;
    }

    void setNewUids(const QStringList& nuids)
    {
        m_newUids = nuids;
    }

    void setOriginalMap(const QMap<QString, QString>& m)
    {
        m_originals = m;
    }

    void setSuspModifiedMap(const QMap<QString, QString>& m)
    {
        m_suspendPatchModifiedElements = m;
    }

    void setSuspNewMap(const QMap<QString, QString>& m)
    {
        m_suspendedPatchNewElements = m;
    }

    void finalizePatchDeserialization();

    void addObjUidToClassUidMapEntry(const QString& objUid, const QString& classUid);

    void setTempTabInstUidVector(const QMap<QString, QVector <QString> > & a)
    {
        m_uidsToTabInstUids = a;
    }

    const QStringList& getLockedUids() const
    {
        return m_lockedUids;
    }

    const QStringList& getDeletedUids() const
    {
        return m_deletedUids;
    }

    const QStringList& getNewUids() const
    {
        return m_newUids;
    }

    const QMap<QString, QString>& getUidToClassUidMap() const
    {
        return m_objUidToClassUid;
    }

    const QMap<QString, QVector <QString> > & getDeletionStruct() const
    {
        return m_uidsToTabInstUids;
    }

    bool suspendPatch(QString&);

private:

    QString serializedElementAsB64(const QString& uid);
    void removeDeletionAction(const QString& uid);

private:
    // the UIDS that are locked in this change
    QStringList m_lockedUids;

    // the original elements, in case we need to re-lock them, we just copy them back and fix the sourceUid
    // they are stored as serialized stuff, and upon serialization they are stored as CDATA.
    // Maps an UID to the serialized XML of the object
    QMap<QString, QString> m_originals;

    // the UIDs of the elements that were created in this patch
    QStringList m_newUids;

    // the UIDs that were deleted from teh version in case of revert
    QStringList m_deletedUids;

    // the delete objects will be placed in here. This will not be serialized, but recreated from the "originals"
    QMap<QString, ObjectWithUid*> m_deletedObjects;

    // if this patch was suspended by the user
    bool m_suspended;

    // the mappings of the deleted objects. These will NOT be serialized, except the table deletions. Then it will be recreated from the originals
    QMap<QString, TableDeletionAction*> m_tableDeletions;

    // class UID to QMap<QString, DeletionAction>
    QMap<QString, QMap<QString, BasicDeletionAction* > > m_deletions;

    /*QMap<QString, DeletionAction<Diagram>*> m_diagramDeletions;
    QMap<QString, DeletionAction<Procedure>*> m_procedureDeletions;
    QMap<QString, DeletionAction<Function>*> m_functionDeletions;
    QMap<QString, DeletionAction<Trigger>*> m_triggerDeletions;
    QMap<QString, DeletionAction<View>*> m_viewDeletions;
    QMap<QString, DeletionAction<UserDataType>*> m_dtDeletions;*/

    //temporayr stuff used for deserialization
    QMap<QString, QString> m_objUidToClassUid; // contains a map of obj uid to class uid just for the patch internals
    QMap<QString, QVector <QString> > m_uidsToTabInstUids;

    // this map contains the elements that were modified in this patch. This is populated when the
    // user has suspended the patch with the elements from the version that were modified (basically
    // the elements of the m_lockedUids are serialized in here)
    QMap<QString, QString> m_suspendPatchModifiedElements;

    // this map contains the elements that were added in the patch. This map is populated
    // when the user suspends a patch. The elements from m_newUids are to be found here
    QMap<QString, QString> m_suspendedPatchNewElements;
};

#endif // CORE_PATCH_H
