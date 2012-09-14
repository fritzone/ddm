#include "core_Trigger.h"
#include "Version.h"
#include "Workspace.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "core_Table.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"

Trigger::Trigger(const QString& name, const QString& uid) : SqlSourceEntity(),
    NamedItem(name), ObjectWithUid(uid),
    m_body(), m_event(), m_ttime(), m_table()
{
}

QStringList Trigger::generateSqlSource(AbstractSqlGenerator * gen, QHash<QString, QString> opts, const Connection* /*dest*/)
{
    return gen->generateTriggerSql(this, opts);
}

void Trigger::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement triggerElement = doc.createElement("Trigger");
    triggerElement.setAttribute("Name", m_name);
    triggerElement.setAttribute("Event", m_event);
    triggerElement.setAttribute("Time", m_ttime);
    triggerElement.setAttribute("Table", m_table);
    triggerElement.setAttribute("uid", getObjectUid());
    triggerElement.setAttribute("class-uid", getClassUid());
    triggerElement.setAttribute("locked", isLocked());
    triggerElement.setAttribute("was-locked", wasLocked());
    triggerElement.setAttribute("source-uid", getSourceUid());

    QDomElement textElement = doc.createElement("Body");
    textElement.setAttribute("Encoded", "Base64");
    QDomCDATASection cdata = doc.createCDATASection(QString(getSql().toUtf8().toBase64()));
    textElement.appendChild(cdata);
    triggerElement.appendChild(textElement);

    QDomElement descElement = doc.createElement("Description");  // description
    QDomText descNode = doc.createTextNode(m_description);
    descElement.appendChild(descNode);
    triggerElement.appendChild(descElement);

    parent.appendChild(triggerElement);
}

QUuid Trigger::getClassUid() const
{
    return QUuid(uidTrigger);
}

CloneableElement* Trigger::clone(Version *sourceVersion, Version *targetVersion)
{
    Trigger* t = new Trigger(getName(), QUuid::createUuid().toString());
    t->m_body = m_body;
    t->m_event = m_event;
    t->m_ttime = m_ttime;
    t->m_table = m_table;
    t->m_description = m_description;
    t->setSourceUid(getObjectUid());
    return t;
}

void Trigger::updateGui()
{
    if(isLocked())
    {
        getLocation()->setIcon(0, IconFactory::getLockedTriggerIcon());
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockTriggerPopupMenu());
    }
    else
    {
        if(wasLocked())
        {
            getLocation()->setIcon(0, IconFactory::getTriggerIcon());
            getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getRelockTriggerPopupMenu());
        }
    }
    TreeItem::updateGui();
}
