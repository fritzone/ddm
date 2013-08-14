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
    QDomElement element = doc.createElement("Procedure");
    element.setAttribute("Name", m_name);
    element.setAttribute("uid", getObjectUid());
    element.setAttribute("class-uid", getClassUid());
    element.setAttribute("locked", lockState() == LockableElement::LOCKED);
    element.setAttribute("was-locked", wasLocked());
    element.setAttribute("source-uid", getSourceUid());
    element.setAttribute("guided", m_guidedCreation);

    if(m_guidedCreation)
    {
        serialize_parameters(doc, element);

        // TODO: Duplicate from the function
        if(m_javaMappedMethod)
        {
            element.setAttribute("java-mapped", true);
            element.setAttribute("java-class", m_javaClassName);
            element.setAttribute("java-method", m_javaMethodName);
        }
    }

    QDomElement textElement = doc.createElement("Sql");
    textElement.setAttribute("Encoded", "Base64");
    QDomCDATASection cdata = doc.createCDATASection(QString(m_sql.toUtf8().toBase64()));
    textElement.appendChild(cdata);
    element.appendChild(textElement);
    parent.appendChild(element);
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
