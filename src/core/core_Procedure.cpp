#include "core_Procedure.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"
#include "uids.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"

Procedure::Procedure(const QString &pname, const QString& uid, Version *v, bool guided) : StoredMethod(pname, uid, v, guided)
{
}

void Procedure::serialize(QDomDocument &doc, QDomElement &parent) const
{
    // TODO: This is almost the same as with the function, refactor
    QDomElement procElement = doc.createElement("Procedure");
    procElement.setAttribute("Name", m_name);
    procElement.setAttribute("uid", getObjectUid());
    procElement.setAttribute("class-uid", getClassUid());
    procElement.setAttribute("locked", lockState() == LockableElement::LOCKED);
    procElement.setAttribute("was-locked", wasLocked());
    procElement.setAttribute("source-uid", getSourceUid());
    procElement.setAttribute("guided", m_guidedCreation);

    if(m_guidedCreation)
    {
        procElement.setAttribute("returns", getReturnType());
        serialize_parameters(doc, procElement);
    }

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

CloneableElement* Procedure::clone(Version* /*sourceVersion*/, Version *targetVersion)
{
    Procedure* newp = new Procedure(getName(), QUuid::createUuid().toString(), targetVersion, m_guidedCreation);
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
