#include "core_Procedure.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"

Procedure::Procedure() : SqlSourceEntity(), NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getProcedure, QString("proc")))
{
}

Procedure::Procedure(const QString &pname) : SqlSourceEntity(), NamedItem(pname)
{

}

QStringList Procedure::generateSqlSource(AbstractSqlGenerator * gen, QHash<QString, QString> /*opts*/, const QString& /*codepage*/)
{

    QStringList r;
    r.append(gen->generateCreateProcedureSql(this, Configuration::instance().sqlOpts()));
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

void Procedure::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement procElement = doc.createElement("Procedure");
    procElement.setAttribute("Name", m_name);
    QDomElement textElement = doc.createElement("Sql");
    QDomCDATASection cdata = doc.createCDATASection(m_sql);
    textElement.appendChild(cdata);
    procElement.appendChild(textElement);
    parent.appendChild(procElement);
}
