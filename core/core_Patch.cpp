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

}

void Patch::addDeletedTable(const QString &uid, TableDeletionAction *td)
{
    m_tableDeletions[uid] = td;
}

TableDeletionAction* Patch::getTDA(const QString &uid)
{
    if(!m_tableDeletions.contains(uid)) return 0;
    TableDeletionAction* tda = m_tableDeletions.value(uid);
    return tda;
}

void Patch::removeTDA(const QString &uid)
{
    if(!m_tableDeletions.contains(uid)) return;
    TableDeletionAction* tda = m_tableDeletions.value(uid);
    m_tableDeletions.remove(uid);
}
