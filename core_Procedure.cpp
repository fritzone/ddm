#include "core_Procedure.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"

Procedure::Procedure() : StoredMethod(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getProcedure, QString("proc")))
{
}

Procedure::Procedure(const QString &pname) : StoredMethod(pname)
{
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
