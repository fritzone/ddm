#include "core_Trigger.h"
#include "Version.h"
#include "Workspace.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "core_Table.h"

Trigger::Trigger(const QString& name, const QString& uid) : SqlSourceEntity(),
    NamedItem(name), ObjectWithUid(uid),
    m_body(), m_event(), m_ttime(), m_table()
{
}

QStringList Trigger::generateSqlSource(AbstractSqlGenerator * gen, QHash<QString, QString> opts, const Connection* dest)
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

    QDomElement textElement = doc.createElement("Body");
    QDomText cdata = doc.createTextNode(m_body);
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
