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

#include <QDateTime>

Patch::Patch(Version *v) : NamedItem("Patch"), ObjectWithUid(QUuid::createUuid().toString(), v), m_lockedUids(), m_originals(), m_suspended(false)
{
    QString finalName = QObject::tr("Patch") + " (" + v->getVersionText() + ") - " + QDateTime::currentDateTime().toString();
    setName(finalName);
    m_version = v;
    MainWindow::instance()->getGuiElements()->createNewPatchItem(this);
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

    m_deletedUids.append(uid);
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
            qDebug() << "Cannot set a b64 encoded stuff";
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
    patchElement.setAttribute("Name", m_name);
    patchElement.setAttribute("uid", getObjectUid());
    patchElement.setAttribute("class-uid", getClassUid().toString());

    {
    // save the locked uids
    QDomElement lockedUids = doc.createElement("LockedUids");
    for(int i=0; i<m_lockedUids.size(); i++)
    {
        QDomElement lockedUid = doc.createElement("uid");
        lockedUid.setAttribute("value", m_lockedUids.at(i));
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
        tableDel.setAttribute("uid", m_tableDeletions.keys().at(i));
        TableDeletionAction* tda = m_tableDeletions[m_tableDeletions.keys().at(i)];
        for(int j=0; j<tda->deletedTableInstances.size(); j++)
        {
            QDomElement tinstDel = doc.createElement("PulledInObjects");
            tinstDel.setAttribute("uid", tda->deletedTableInstances.at(j)->getObjectUid());
            tableDel.appendChild(tinstDel);
        }
        tableDeletions.appendChild(tableDel);
    }
    patchElement.appendChild(tableDeletions);
    }

    parent.appendChild(patchElement);
}
