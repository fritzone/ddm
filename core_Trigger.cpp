#include "core_Trigger.h"
#include "Version.h"
#include "Workspace.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Table.h"

Trigger::Trigger() : SqlSourceEntity(),
    NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getTrigger, QString("trig"))),
    m_body(), m_event(), m_ttime(), m_table(0)
{
}

Trigger::Trigger(const QString& name) : SqlSourceEntity(),
    NamedItem(name), m_body(), m_event(), m_ttime(), m_table(0)
{
}

QStringList Trigger::generateSqlSource(AbstractSqlGenerator * gen, QHash<QString, QString> opts, const QString& /*codepage*/)
{
    return gen->generateTriggerSql(this, opts);
}

void Trigger::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement triggerElement = doc.createElement("Trigger");
    triggerElement.setAttribute("Name", m_name);
    triggerElement.setAttribute("Event", m_event);
    triggerElement.setAttribute("Time", m_ttime);
    triggerElement.setAttribute("Table", getName());

    QDomElement textElement = doc.createElement("Body");
    QDomCDATASection cdata = doc.createCDATASection(m_body);
    textElement.appendChild(cdata);
    triggerElement.appendChild(textElement);
    parent.appendChild(triggerElement);
}
