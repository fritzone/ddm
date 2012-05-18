#include "core_Function.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"
#include "uids.h"

Function::Function(const QString &pname, const QString& uid) : StoredMethod(pname), ObjectWithUid(uid)
{
}

void Function::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement element = doc.createElement("Function");
    element.setAttribute("Name", m_name);
    element.setAttribute("uid", getObjectUid());
    element.setAttribute("class-uid", getClassUid());

    QDomElement textElement = doc.createElement("Sql");
    QDomText cdata = doc.createTextNode(m_sql);
    textElement.appendChild(cdata);
    element.appendChild(textElement);
    parent.appendChild(element);
}

QUuid Function::getClassUid() const
{
    return QUuid(uidFunction);
}
