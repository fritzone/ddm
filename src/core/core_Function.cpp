#include "core_Function.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"
#include "uids.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"

Function::Function(const QString &pname, const QString& uid, Version *v, bool guided) : StoredMethod(pname, uid, v, guided)
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
    element.setAttribute("guided", m_guidedCreation);

    if(m_guidedCreation)
    {
        element.setAttribute("returns", getReturnType());
        serialize_parameters(doc, element);

        QDomElement descEl = doc.createElement("ReturnDescription");
        QDomCDATASection cdDesc = doc.createCDATASection(m_returnDesc);
        descEl.appendChild(cdDesc);
        element.appendChild(descEl);

        if(m_javaMappedMethod)
        {
            element.setAttribute("java-mapped", true);
            element.setAttribute("java-class", m_javaClassName);
            element.setAttribute("java-method", m_javaMethodName);

            element.setAttribute("java-returns", m_javaReturns);
        }
    }

    QDomElement sqlElement = doc.createElement("Sql");
    sqlElement.setAttribute("Encoded", "Base64");
    QDomCDATASection cdata = doc.createCDATASection(QString(m_sql.toUtf8().toBase64()));
    sqlElement.appendChild(cdata);
    element.appendChild(sqlElement);

    parent.appendChild(element);
}

QUuid Function::getClassUid() const
{
    return QUuid(uidFunction);
}

CloneableElement* Function::clone(Version* /*sourceVersion*/, Version *targetVersion)
{
    Function* newp = new Function(getName(), QUuid::createUuid().toString(), targetVersion, m_guidedCreation);
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
