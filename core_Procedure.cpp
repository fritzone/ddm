#include "core_Procedure.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"

Procedure::Procedure() : SqlSourceEntity(), NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (NameGenerator::itemGetter)&Version::getProcedure, QString("proc")))
{
}

QStringList Procedure::generateSqlSource(AbstractSqlGenerator *, QHash<QString, QString>, const QString &codepage)
{
    QStringList r;
    r.append(m_sql);
    return r;
}

void Procedure::setSql(const QString &s)
{
    m_sql = s;
}

void Procedure::rename(const QString &n)
{
    setName(n);
    getLocation()->setText(0, n);
}
