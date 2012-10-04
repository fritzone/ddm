#include "core_Patch.h"
#include "UidWarehouse.h"
#include "core_CloneableElement.h"
#include "core_ObjectWithUid.h"
#include "uids.h"
#include "DeserializationFactory.h"
#include "core_Table.h"
#include "Version.h"
#include "MainWindow.h" // TODO: horrible
#include "GuiElements.h"
#include "core_TableInstance.h"
#include "core_Diagram.h"
#include "core_Procedure.h"
#include "core_Function.h"
#include "core_Trigger.h"
#include "core_View.h"
#include "core_UserDataType.h"
#include "core_LockableElement.h"
#include "NamedItem.h"

#include <QDateTime>

Patch::Patch(Version *v, bool init) : NamedItem("Patch"), ObjectWithUid(QUuid::createUuid().toString(), v), m_lockedUids(), m_originals(), m_suspended(false)
{
    m_version = v;

    if(!init)
    {
        QString finalName = QObject::tr("Patch") + " (" + v->getVersionText() + ") - " + QDateTime::currentDateTime().toString();
        setName(finalName);
        MainWindow::instance()->getGuiElements()->createNewPatchItem(this);
    }
}

void Patch::addElement(const QString &uid)
{
    ObjectWithUid* element = UidWarehouse::instance().getElement(uid);
    if(!element)
    {
        qDebug() << "no element for " << uid;
        return;
    }
    if(m_lockedUids.contains(uid, Qt::CaseInsensitive))
    {
        qDebug() << "element already here " << uid;
        return;
    }
    m_lockedUids.append(uid);
    SerializableElement* e = dynamic_cast<SerializableElement*>(element);
    if(!e)
    {
        return;
    }

    QDomDocument doc("PatchData");
    QDomElement root = doc.createElement("OriginalElement");
    e->serialize(doc, root);
    doc.appendChild(root);
    QString text = doc.toString();

    // convert the text to bas64 encoding
    text = QString(text.toUtf8().toBase64());
    m_originals.insert(uid, text);

    qDebug() << "added " << uid;
}

void Patch::addNewElement(const QString &uid)
{
    m_newUids.append(uid);
}

void Patch::markElementForDeletion(const QString &uid)
{
    ObjectWithUid* obj = UidWarehouse::instance().getElement(uid);
    dynamic_cast<LockableElement*>(obj)->deleteObject();

    if(!obj) return;

    if(m_deletedUids.indexOf(uid) == -1)
    {
        m_deletedUids.append(uid);
    }
    m_deletedObjects.insert(uid, obj);

    qDebug() << "mark for deletion: " << uid;
    addElement(uid);
}

void Patch::removeElement(const QString &uid)
{
    if(m_lockedUids.contains(uid))
    {
        m_lockedUids.removeOne(uid);
    }

    if(m_originals.contains(uid))
    {
        QDomDocument a("PatchData");
        QByteArray encoded = QByteArray(m_originals.value(uid).toLocal8Bit());

        QString err;
        QString fromb64 = QString(QByteArray::fromBase64(encoded));
        if(!a.setContent(fromb64, &err))
        {
            qDebug() << "Cannot set a b64 encoded stuff: " << err;
            return;
        }
        QString node = a.documentElement().nodeName();
        if(node != "OriginalElement")
        {
            return;
        }

        // now find the type based on the class-uid and replace the element with the original that was there
        QString class_uid = a.documentElement().firstChild().toElement().attribute("class-uid");
        if(class_uid == uidTable)
        {
            Table* tab = DeserializationFactory::createTable(m_version->getProject()->getEngine(), m_version, a, a.documentElement().firstChild().toElement());
            m_version->setupForeignKeyRelationshipsForATable(tab);
            m_version->replaceTable(uid, tab);
        }

        if(class_uid == uidTableInstance)
        {
            TableInstance* tinst = DeserializationFactory::createTableInstance(m_version, a, a.documentElement().firstChild().toElement());
            m_version->replaceTableInstance(uid, tinst);
        }

        m_originals.remove(uid);
    }
}

QUuid Patch::getClassUid() const
{
    return QUuid(uidPatch);
}

ObjectWithUid* Patch::getDeletedObject(const QString& uid)
{
    if(!m_deletedUids.contains(uid)) return 0;
    if(!m_deletedObjects.keys().contains(uid)) return 0;
    return m_deletedObjects.value(uid);
}

void Patch::undeleteObject(const QString &uid)
{
    if(!m_deletedUids.contains(uid)) return;
    if(!m_deletedObjects.keys().contains(uid)) return;

    m_deletedUids.removeOne(uid);
    m_deletedObjects.remove(uid);

    ObjectWithUid* obj = UidWarehouse::instance().getElement(uid);
    if(!obj) return;
    dynamic_cast<LockableElement*>(obj)->undeleteObject();
    qDebug() << "undeleted" << uid;

}

void Patch::addDeletedTable(const QString &uid, TableDeletionAction *td)
{
    m_tableDeletions[uid] = td;
    // and also pull in the deleted table instances
    for(int i=0; i<td->deletedTableInstances.size(); i++)
    {
        markElementForDeletion(td->deletedTableInstances.at(i)->getObjectUid());
    }

}

void Patch::addDeletedDiagram(const QString &uid, DiagramDeletionAction *dda)
{
    m_diagramDeletions[uid] = dda;
}

void Patch::addDeletedProcedure(const QString &uid, ProcedureDeletionAction *pda)
{
    m_procedureDeletions[uid] = pda;
}

void Patch::addDeletedFunction(const QString &uid, FunctionDeletionAction *pda)
{
    m_functionDeletions[uid] = pda;
}

void Patch::addDeletedView(const QString &uid, ViewDeletionAction *vda)
{
    m_viewDeletions[uid] = vda;
}

void Patch::addDeletedDataType(const QString &uid, DataTypeDeletionAction *dtda)
{
    m_dtDeletions[uid] = dtda;
}

void Patch::addDeletedTrigger(const QString &uid, TriggerDeletionAction *tda)
{
    m_triggerDeletions[uid] = tda;
}

TableDeletionAction* Patch::getTDA(const QString &uid)
{
    if(!m_tableDeletions.contains(uid)) return 0;
    TableDeletionAction* tda = m_tableDeletions.value(uid);
    return tda;
}

DiagramDeletionAction* Patch::getDDA(const QString &uid)
{
    if(!m_diagramDeletions.contains(uid)) return 0;
    DiagramDeletionAction* dda = m_diagramDeletions.value(uid);
    return dda;
}

ProcedureDeletionAction* Patch::getPDA(const QString &uid)
{
    if(!m_procedureDeletions.contains(uid)) return 0;
    ProcedureDeletionAction* pda = m_procedureDeletions.value(uid);
    return pda;
}

FunctionDeletionAction* Patch::getFDA(const QString &uid)
{
    if(!m_functionDeletions.contains(uid)) return 0;
    FunctionDeletionAction* fda = m_functionDeletions.value(uid);
    return fda;
}

TriggerDeletionAction* Patch::getTrDA(const QString &uid)
{
    if(!m_triggerDeletions.contains(uid)) return 0;
    TriggerDeletionAction* tda = m_triggerDeletions.value(uid);
    return tda;
}

ViewDeletionAction* Patch::getVDA(const QString &uid)
{
    if(!m_viewDeletions.contains(uid)) return 0;
    ViewDeletionAction* vda = m_viewDeletions.value(uid);
    return vda;
}

DataTypeDeletionAction* Patch::getDtDA(const QString &uid)
{
    if(!m_dtDeletions.contains(uid)) return 0;
    DataTypeDeletionAction* dtda = m_dtDeletions.value(uid);
    return dtda;
}

void Patch::removeTDA(const QString &uid)
{
    if(!m_tableDeletions.contains(uid)) return;
    TableDeletionAction* tda = m_tableDeletions.value(uid);
    m_tableDeletions.remove(uid);
}

bool Patch::elementWasNewInThisPatch(const QString &uid)
{
    int idx = m_newUids.indexOf(uid);
    return idx != -1;
}

bool Patch::elementWasLockedInThisPatch(const QString &uid)
{
    int idx = m_lockedUids.indexOf(uid);
    return idx != -1;
}

bool Patch::removeNewElementBecauseOfDeletion(const QString& uid)
{
    if(elementWasNewInThisPatch(uid))
    {
        m_newUids.removeOne(uid);
        if(m_originals.contains(uid))
        {
            m_originals.remove(uid);
        }
    }
    return true;
}

void Patch::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement patchElement = doc.createElement("Patch");
    patchElement.setAttribute("name", m_name);
    patchElement.setAttribute("uid", getObjectUid());
    patchElement.setAttribute("class-uid", getClassUid().toString());
    patchElement.setAttribute("suspended", m_suspended);

    {
    // save the locked uids
    QDomElement lockedUids = doc.createElement("LockedUids");
    for(int i=0; i<m_lockedUids.size(); i++)
    {
        QDomElement lockedUid = doc.createElement("uid");
        lockedUid.setAttribute("value", m_lockedUids.at(i));
        ObjectWithUid* obj = UidWarehouse::instance().getElement(m_lockedUids.at(i));
        lockedUid.setAttribute("class-uid", obj->getClassUid().toString());
        lockedUids.appendChild(lockedUid);
    }
    patchElement.appendChild(lockedUids);
    }

    {
    // save the new uids
    QDomElement newUids = doc.createElement("NewUids");
    for(int i=0; i<m_newUids.size(); i++)
    {
        QDomElement newUid = doc.createElement("uid");
        ObjectWithUid* obj = UidWarehouse::instance().getElement(m_newUids.at(i));
        newUid.setAttribute("class-uid", obj->getClassUid().toString());
        newUid.setAttribute("value", m_newUids.at(i));
        newUids.appendChild(newUid);
    }
    patchElement.appendChild(newUids);
    }

    {
    // save the deleted uids
    QDomElement deletedUids = doc.createElement("DeletedUids");
    for(int i=0; i<m_deletedUids.size(); i++)
    {
        QDomElement deletedUid = doc.createElement("uid");
        deletedUid.setAttribute("value", m_deletedUids.at(i));
        ObjectWithUid* obj = UidWarehouse::instance().getElement(m_deletedUids.at(i));
        deletedUid.setAttribute("class-uid", obj->getClassUid().toString());
        deletedUids.appendChild(deletedUid);
    }
    patchElement.appendChild(deletedUids);
    }

    {
    // the originals
    QDomElement originals = doc.createElement("Originals");
    for(int i=0; i<m_originals.keys().size(); i++)
    {
        QDomElement original = doc.createElement("Original");
        original.setAttribute("uid", m_originals.keys().at(i));
        ObjectWithUid* obj = UidWarehouse::instance().getElement(m_originals.keys().at(i));
        original.setAttribute("class-uid", obj->getClassUid().toString());
        QDomCDATASection cd = doc.createCDATASection(m_originals[m_originals.keys().at(i)]);
        original.appendChild(cd);
        originals.appendChild(original);
    }
    patchElement.appendChild(originals);
    }

    {
    // and just to be sure all the TableDeletionActions
    QDomElement tableDeletions = doc.createElement("TableDeletions");
    for(int i=0; i<m_tableDeletions.keys().size(); i++)
    {
        QDomElement tableDel = doc.createElement("MainObject");
        QString mainUid = m_tableDeletions.keys().at(i);
        tableDel.setAttribute("uid", mainUid);
        TableDeletionAction* tda = m_tableDeletions[m_tableDeletions.keys().at(i)];
        if(tda->parentTable)
        {
            tableDel.setAttribute("parent-uid", tda->parentTable->getObjectUid());
        }
        for(int j=0; j<tda->deletedTableInstances.size(); j++)
        {
            QDomElement tinstDel = doc.createElement("PulledInObjects");
            QString pulledInUid = tda->deletedTableInstances.at(j)->getObjectUid();
            if(mainUid != pulledInUid)
            {
                tinstDel.setAttribute("uid", pulledInUid);
                tableDel.appendChild(tinstDel);
            }
        }
        tableDeletions.appendChild(tableDel);
    }
    patchElement.appendChild(tableDeletions);
    }

    parent.appendChild(patchElement);
}

void Patch::finalizePatchDeserialization()
{
    // create the m_deletedObjects map... ie. deserialize the elements from the originals and fill up this map
    for(int i=0; i<m_deletedUids.size(); i++)
    {
        QString uid = m_deletedUids.at(i);
        if(m_originals.contains(uid))
        {
            QString decoded = QString(QByteArray::fromBase64(m_originals[uid].toLocal8Bit()));
            QString classUid = m_objUidToClassUid[uid];
            if(classUid.length() == 0)
            {
                qDebug() << "No class uid for " << uid;
                continue;
            }
            ObjectWithUid* o = DeserializationFactory::createElementForClassUid(classUid, decoded, version());
            if(o)
            {
                m_deletedObjects.insert(uid, o);
                dynamic_cast<LockableElement*>(o)->deleteObject();
            }
        }
    }

    // now fix the table of the deleted table instances in case they reference a table that was deleted
    for(int i=0; i<m_deletedObjects.keys().size(); i++)
    {
        TableInstance* tinst = dynamic_cast<TableInstance*>(m_deletedObjects[m_deletedObjects.keys().at(i)]);
        if(tinst)
        {
            QString tn = tinst->getTableName();
            if(!tn.isEmpty())   // means, could not fid an object in the version, it must have been in the patch, already deleted
            {
                for(int j=0; j<m_deletedObjects.size(); j++)
                {
                    NamedItem* ni = dynamic_cast<NamedItem*>(m_deletedObjects[m_deletedObjects.keys().at(j)]);
                    if(ni->getName() == tn && m_deletedObjects[m_deletedObjects.keys().at(j)]->getClassUid() == uidTable)
                    {
                        tinst->forceSetTable(dynamic_cast<Table*>(m_deletedObjects[m_deletedObjects.keys().at(j)]));
                    }
                }
            }
        }
    }

    //now create the TableDeletionAction map
    for(int i=0; i<m_uidsToTabInstUids.keys().size(); i++)
    {
        TableDeletionAction* tda = new TableDeletionAction(0);
        bool tableWasSet = false;
        if(m_objUidToClassUid[m_uidsToTabInstUids.keys().at(i)].toUpper() == uidTable.toUpper())    // create a table
        {
            // get it from the m_deletedObjects
            QString uid = m_uidsToTabInstUids.keys().at(i);
            ObjectWithUid* o = m_deletedObjects[uid];
            if(o)
            {
                Table* t = dynamic_cast<Table*>(o);
                tda->deletedTable = t;
                if(t)
                {
                    tda->parentTable = t->parent();
                    tableWasSet = true;
                }
            }
        }

        // regardless what was there fill up the table instances of the tda
        const QVector<QString>& vec = m_uidsToTabInstUids[m_uidsToTabInstUids.keys().at(i)];
        for(int j=0; j<vec.size(); j++)
        {
            QString uid = vec.at(j);
            ObjectWithUid* o = m_deletedObjects[uid];
            if(o)
            {
                tda->deletedTableInstances.append(dynamic_cast<TableInstance*>(o));
            }
        }

        // if a table was not set we need to add in the main element in the TDA deletedTableInstances
        if(!tableWasSet)
        {
            QString uid = m_uidsToTabInstUids.keys().at(i);
            ObjectWithUid* o = m_deletedObjects[uid];
            if(o)
            {
                tda->deletedTableInstances.prepend(dynamic_cast<TableInstance*>(o));
            }
        }
        m_tableDeletions.insert(m_uidsToTabInstUids.keys().at(i), tda);
    }

    // now create the DiagramDeletion map
    for(int i=0; i<m_deletedUids.size(); i++)
    {
        QString uid = m_deletedUids.at(i);
        QString classUid = m_objUidToClassUid[uid];
        if(classUid.toUpper() == uidDiagram.toUpper())
        {
            DiagramDeletionAction* dda = new DiagramDeletionAction;
            ObjectWithUid* o = (m_deletedObjects[uid]);
            dda->deletedDiagram = dynamic_cast<Diagram*>(o);
            m_diagramDeletions.insert(uid, dda);
        }
    }

    // now create the ProcedureDeletion map
    for(int i=0; i<m_deletedUids.size(); i++)
    {
        QString uid = m_deletedUids.at(i);
        QString classUid = m_objUidToClassUid[uid];
        if(classUid.toUpper() == uidProcedure.toUpper())
        {
            ProcedureDeletionAction* pda = new ProcedureDeletionAction;
            ObjectWithUid* o = (m_deletedObjects[uid]);
            pda->deletedProcedure = dynamic_cast<Procedure*>(o);
            m_procedureDeletions.insert(uid, pda);
        }
    }

    // now create the FunctionDeletion map
    for(int i=0; i<m_deletedUids.size(); i++)
    {
        QString uid = m_deletedUids.at(i);
        QString classUid = m_objUidToClassUid[uid];
        if(classUid.toUpper() == uidFunction.toUpper())
        {
            FunctionDeletionAction* fda = new FunctionDeletionAction;
            ObjectWithUid* o = (m_deletedObjects[uid]);
            fda->deletedFunction = dynamic_cast<Function*>(o);
            m_functionDeletions.insert(uid, fda);
        }
    }

    // now create the TriggerDeletion map
    for(int i=0; i<m_deletedUids.size(); i++)
    {
        QString uid = m_deletedUids.at(i);
        QString classUid = m_objUidToClassUid[uid];
        if(classUid.toUpper() == uidTrigger.toUpper())
        {
            TriggerDeletionAction* tda = new TriggerDeletionAction;
            ObjectWithUid* o = (m_deletedObjects[uid]);
            tda->deletedTrigger= dynamic_cast<Trigger*>(o);
            m_triggerDeletions.insert(uid, tda);
        }
    }

    // now create the ViewDeletion map
    for(int i=0; i<m_deletedUids.size(); i++)
    {
        QString uid = m_deletedUids.at(i);
        QString classUid = m_objUidToClassUid[uid];
        if(classUid.toUpper() == uidView.toUpper())
        {
            ViewDeletionAction* vda = new ViewDeletionAction;
            ObjectWithUid* o = (m_deletedObjects[uid]);
            vda->deletedView = dynamic_cast<View*>(o);
            m_viewDeletions.insert(uid, vda);
        }
    }

    // now create the DataTypeDeletion map
    for(int i=0; i<m_deletedUids.size(); i++)
    {
        QString uid = m_deletedUids.at(i);
        QString classUid = m_objUidToClassUid[uid];
        if(classUid.toUpper() == uidNumericDT.toUpper()
            || classUid.toUpper() == uidStringDT.toUpper()
            || classUid.toUpper() == uidDateTimeDT.toUpper()
            || classUid.toUpper() == uidBooleanDT.toUpper()
            || classUid.toUpper() == uidBlobDT.toUpper()
            || classUid.toUpper() == uidMiscDT.toUpper()
            || classUid.toUpper() == uidSpatialDT.toUpper())
        {
            DataTypeDeletionAction* dtda = new DataTypeDeletionAction;
            ObjectWithUid* o = (m_deletedObjects[uid]);
            dtda->deletedDataType = dynamic_cast<UserDataType*>(o);
            m_dtDeletions.insert(uid, dtda);
        }
    }

}
