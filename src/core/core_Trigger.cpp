#include "core_Trigger.h"
#include "Version.h"
#include "Workspace.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "core_Table.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"

#include <QCryptographicHash>

Trigger::Trigger(const QString& name, const QString& uid, Version* v) : SqlSourceTreeItem(),
    NamedItem(name), VersionElement(uid, v),
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
    triggerElement.setAttribute("uid", getObjectUid().toString());
    triggerElement.setAttribute("class-uid", getClassUid().toString());
    triggerElement.setAttribute("locked", lockState() == LockableElement::LOCKED);
    triggerElement.setAttribute("was-locked", wasLocked());
    triggerElement.setAttribute("source-uid", getSourceUid().toString());

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

CloneableElement* Trigger::clone(Version* /*sourceVersion*/, Version *targetVersion)
{
    Trigger* t = new Trigger(getName(), QUuid::createUuid().toString(), targetVersion);
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
    if(lockState() == LockableElement::LOCKED)
    {
        getLocation()->setIcon(0, IconFactory::getLockedTriggerIcon());
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockTriggerPopupMenu());
    }
    else
    if(lockState() == LockableElement::FINAL_LOCK)
    {
        getLocation()->setIcon(0, IconFactory::getFinalLockedTriggerIcon());
        getLocation()->setPopupMenu(0);
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

QString Trigger::getSqlHash() const
{
    QString s = m_body + m_event + m_ttime + m_table;
    QString spaceless = "";
    for(int i=0; i<s.size(); i++)
    {
        if(!s.at(i).isSpace())
        {
            spaceless += s.at(i);
        }
    }
    QString hash = QString(QCryptographicHash::hash((spaceless.toUpper().toLocal8Bit()),QCryptographicHash::Md5).toHex());
    return hash;
}

const QVector<Trigger *> &Trigger::family(const Trigger& t)
{
    return t.version()->getTriggers();
}
