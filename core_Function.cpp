#include "core_Function.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"

Function::Function() : StoredMethod(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getFunction, QString("func")))
{
}

Function::Function(const QString &pname) : StoredMethod(pname)
{
}

void Function::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement procElement = doc.createElement("Function");
    procElement.setAttribute("Name", m_name);
    QDomElement textElement = doc.createElement("Sql");
    QDomCDATASection cdata = doc.createCDATASection(m_sql);
    textElement.appendChild(cdata);
    procElement.appendChild(textElement);
    parent.appendChild(procElement);
}

