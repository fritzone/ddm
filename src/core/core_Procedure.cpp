#include "core_Procedure.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"
#include "uids.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"

Procedure::Procedure(const QString &pname, const QString& uid, Version *v) : StoredMethod(pname, uid, v)
{
}

void Procedure::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement procElement = doc.createElement("Procedure");
    procElement.setAttribute("Name", m_name);
    procElement.setAttribute("uid", getObjectUid());
    procElement.setAttribute("class-uid", getClassUid());
    procElement.setAttribute("locked", lockState() == LockableElement::LOCKED);
    procElement.setAttribute("was-locked", wasLocked());
    procElement.setAttribute("source-uid", getSourceUid());

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

CloneableElement* Procedure::clone(Version */*sourceVersion*/, Version *targetVersion)
{
    Procedure* newp = new Procedure(getName(), QUuid::createUuid().toString(), targetVersion );
    newp->setSql(getSql());
    newp->setSourceUid(getObjectUid());
    return newp;
}

void Procedure::updateGui()
{
    if(lockState() == LockableElement::LOCKED)
    {
        getLocation()->setIcon(0, IconFactory::getLockedProcedureIcon());
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockProcedurePopupMenu());
    }
    else
    if(lockState() == LockableElement::FINAL_LOCK)
    {
        getLocation()->setIcon(0, IconFactory::getFinalLockedProcedureIcon());
        getLocation()->setPopupMenu(0);
    }
    else
    {
        if(wasLocked())
        {
            getLocation()->setIcon(0, IconFactory::getProcedureIcon());
            getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getRelockProcedurePopupMenu());
        }
    }
    TreeItem::updateGui();
}
