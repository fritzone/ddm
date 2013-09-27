#include "core_View.h"
#include "qbr_QueryAsGenerator.h"
#include "qbr_SelectQuery.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "uids.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"
#include "qbr_SelectQueryFromComponent.h"
#include "core_Table.h"

#include <QCryptographicHash>

View::View(bool manual, QString uid, Version *v) :
    SqlSourceTreeItem(),
    NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getView, QString("my_view"))),
    VersionElement(uid, v),
    ObjectWithSpInstances(),
    m_columNames(), m_manual(manual)
{
    m_helper = new QueryGraphicsHelper();
    m_selectQuery = new SelectQuery(m_helper, 0, this, v);
    QueryAsGenerator::instance().initNewQuery(m_selectQuery);
    m_helper->setQuery(m_selectQuery);
}

View::View(Version*v, bool manual, QString uid) :
    SqlSourceTreeItem(),
    NamedItem(NameGenerator::getUniqueName(v, (itemGetter)&Version::getView, QString("my_view"))),
    VersionElement(uid, v),
    m_columNames(), m_manual(manual)
{
    m_helper = new QueryGraphicsHelper();
}

void View::secondStepClone()
{
    m_helper = new QueryGraphicsHelper();
    QueryAsGenerator::instance().initNewQuery(m_selectQuery);
    m_helper->setQuery(m_selectQuery);
}

void View::finalizeViewDeserialization()
{
    QueryAsGenerator::instance().initNewQuery(m_selectQuery);
}

QStringList View::generateSqlSource(AbstractSqlGenerator * gen, QHash<QString, QString> opts, const Connection* /*dest*/)
{
    QStringList createSql = gen->generateCreateViewSql(this, opts);
    return createSql;
}

void View::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement viewElement = doc.createElement("View");
    viewElement.setAttribute("Name", m_name);
    viewElement.setAttribute("Manual", m_manual);
    viewElement.setAttribute("uid", getObjectUid().toString());
    viewElement.setAttribute("class-uid", getClassUid().toString());
    viewElement.setAttribute("source-uid", getSourceUid().toString());
    viewElement.setAttribute("locked", lockState() == LockableElement::LOCKED);
    viewElement.setAttribute("was-locked", wasLocked());

    if(m_manual)
    {
        QDomElement textElement = doc.createElement("Sql");
        QDomCDATASection cdata = doc.createCDATASection(m_sql);
        textElement.appendChild(cdata);
        viewElement.appendChild(textElement);
    }
    else
    {
        QDomElement columns = doc.createElement("Columns");
        for(int i=0; i<m_columNames.size(); i++)
        {
            QDomElement column = doc.createElement("Column");
            column.setAttribute("Name", m_columNames.at(i));
            columns.appendChild(column);
        }
        viewElement.appendChild(columns);
        // and now render the query
        m_selectQuery->serialize(doc, viewElement);
    }

    // save the sps
    {
    QDomElement spsElement = doc.createElement("SpInstances");
    serialize_spinstances(doc, spsElement);
    viewElement.appendChild(spsElement);
    }

    parent.appendChild(viewElement);
}

QUuid View::getClassUid() const
{
    return QUuid(uidView);
}

CloneableElement* View::clone(Version *sourceVersion, Version *targetVersion)
{
    View* nv = new View(targetVersion, m_manual, QUuid::createUuid().toString());
    nv->setName(m_name);
    nv->setSourceUid(getObjectUid());
    if(m_manual)
    {
        nv->setSql(m_sql);
    }
    else
    {
        SelectQuery* newQuery = dynamic_cast<SelectQuery*>(m_selectQuery->clone(sourceVersion, targetVersion));
        nv->setQuery(newQuery);
        nv->m_columNames = m_columNames;
        nv->secondStepClone();
    }

    return nv;
}

void View::updateGui()
{
    if(lockState() == LockableElement::LOCKED)
    {
        getLocation()->setIcon(0, IconFactory::getLockedViewIcon());
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockViewPopupMenu());
    }
    else
    if(lockState() == LockableElement::FINAL_LOCK)
    {
        getLocation()->setIcon(0, IconFactory::getFinalLockedViewIcon());
        getLocation()->setPopupMenu(0);
    }
    else
    {
        if(wasLocked())
        {
            getLocation()->setIcon(0, IconFactory::getViewIcon());
            getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getRelockViewPopupMenu());
        }
    }
    TreeItem::updateGui();
}

QString View::getSqlHash() const
{
    QString sql = m_manual?m_sql:m_selectQuery->get();
    return getHash(sql);
}

QString View::getHash(const QString& s) const
{
    QString spaceless = "";
    for(int i=0; i<s.size(); i++)
    {
        if(!s.at(i).isSpace())
        {
            spaceless += s.at(i);
        }
    }
    QString hash = QString(QCryptographicHash::hash((spaceless.toUpper().toLocal8Bit()),QCryptographicHash::Md5).toHex());
    return hash;
}

QString View::getHashForCreationStatement() const
{
    return getHash(getCreationStatement());
}

QString View::getCreationStatement() const
{
    QString result = "";
    QString sql = m_manual?m_sql:m_selectQuery->get();
    int asPosition = sql.indexOf("AS", Qt::CaseInsensitive);
    if(m_manual)
    {
        while(asPosition > -1 && asPosition < sql.length())
        {
            asPosition += 2;
            int t = asPosition;
            while(sql.at(asPosition).isSpace()) asPosition ++;
            if(t == asPosition) // no space around as
            {
                asPosition = sql.indexOf("AS", asPosition, Qt::CaseInsensitive);
            }
            else
            {
                // now asPosition should be pointing to the "SELECT"
                QString nextW = "";
                int selP = asPosition;
                while(!sql.at(asPosition).isSpace())
                {
                    nextW += sql.at(asPosition);
                    asPosition ++;
                }
                if(nextW.toUpper() == "SELECT")
                {
                    // found the select after the AS
                    result = sql.mid(selP);
                    return !m_manual?result.toUpper():result;
                }
            }
        }
    }
    else
    {
        result = sql;
    }
    return result;
}

QVector<const Table*> View::getSourceTables() const
{
    QVector<const Table*> result;
    if(m_manual)
    {
        return result;
    }
    else
    {
        if(m_selectQuery->hasFrom())
        {
            SelectQueryFromComponent* fr = m_selectQuery->getFrom();
            result = fr->getTables();
        }
    }
    return result;
}

bool View::usesTable(const Table *tab) const
{
    QVector<const Table*> tabs = getSourceTables();
    for(int i=0; i<tabs.size(); i++)
    {
        if(tabs[i]->getName() == tab->getName())
        {
            return true;
        }
    }
    return false;
}

const QVector<View *> &View::family(const View& v)
{
    return v.version()->getViews();
}
