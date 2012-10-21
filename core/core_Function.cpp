#include "core_Function.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"
#include "uids.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"

Function::Function(const QString &pname, const QString& uid, Version *v) : StoredMethod(pname, uid, v)
{
}

void Function::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement element = doc.createElement("Function");
    element.setAttribute("Name", m_name);
    element.setAttribute("uid", getObjectUid());
    element.setAttribute("class-uid", getClassUid());
    element.setAttribute("source-uid", getSourceUid());
    element.setAttribute("locked", lockState() == LockableElement::LOCKED);
    element.setAttribute("was-locked", wasLocked());

    QDomElement textElement = doc.createElement("Sql");
    textElement.setAttribute("Encoded", "Base64");
    QDomCDATASection cdata = doc.createCDATASection(QString(m_sql.toUtf8().toBase64()));
    textElement.appendChild(cdata);
    element.appendChild(textElement);
    parent.appendChild(element);
}

QUuid Function::getClassUid() const
{
    return QUuid(uidFunction);
}

CloneableElement* Function::clone(Version *sourceVersion, Version *targetVersion)
{
    Function* newp = new Function(getName(), QUuid::createUuid().toString(), targetVersion );
    newp->setSql(getSql());
    newp->setSourceUid(getObjectUid());
    return newp;
}

void Function::updateGui()
{
    if(lockState() == LockableElement::LOCKED)
    {
        getLocation()->setIcon(0, IconFactory::getLockedFunctionTreeIcon());
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockFunctionPopupMenu());
    }
    else
    if(lockState() == LockableElement::FINAL_LOCK)
    {
        getLocation()->setIcon(0, IconFactory::getFinalLockedFunctionTreeIcon());
        getLocation()->setPopupMenu(0);
    }
    else
    {
        if(wasLocked())
        {
            getLocation()->setIcon(0, IconFactory::getFunctionTreeIcon());
            getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getRelockFunctionPopupMenu());
        }
    }
    TreeItem::updateGui();
}
