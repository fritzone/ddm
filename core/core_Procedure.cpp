#include "core_Procedure.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"
#include "uids.h"

Procedure::Procedure(const QString &pname, const QString& uid) : StoredMethod(pname),
    ObjectWithUid(uid)
{
}

void Procedure::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement procElement = doc.createElement("Procedure");
    procElement.setAttribute("Name", m_name);
    procElement.setAttribute("uid", getObjectUid());
    procElement.setAttribute("class-uid", getClassUid());

    QDomElement textElement = doc.createElement("Sql");
    textElement.setAttribute("Encoded", "Base64");
    QDomCDATASection cdata = doc.createCDATASection(QString(m_sql.toUtf8().toBase64()));
    textElement.appendChild(cdata);
    procElement.appendChild(textElement);
    parent.appendChild(procElement);
}

QUuid Procedure::getClassUid() const
{
    return QUuid(uidProcedure);
}
