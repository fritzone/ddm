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

    m_originals.insert(uid, text);
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
        QString err;
        if(!a.setContent(m_originals.value(uid), &err))
        {
            return;
        }
        QString node = a.documentElement().nodeName();
        if(node != "OriginalElement")
        {
            return;
        }

        // now find the type based on the class-uid
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

void Patch::serialize(QDomDocument &doc, QDomElement &parent) const
{

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
