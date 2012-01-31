#include "core_Trigger.h"
#include "Version.h"
#include "Workspace.h"
#include "NameGenerator.h"

Trigger::Trigger() : SqlSourceEntity(), NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getTrigger, QString("trig")))
{
}

QStringList Trigger::generateSqlSource(AbstractSqlGenerator *, QHash<QString, QString>, const QString &codepage)
{
    return QStringList();
}

void Trigger::serialize(QDomDocument &doc, QDomElement &parent) const
{

}
