#include "core_View.h"
#include "qbr_QueryAsGenerator.h"
#include "qbr_SelectQuery.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"
#include "db_AbstractSQLGenerator.h"
#include "uids.h"

View::View(bool manual, QString uid) :
    SqlSourceEntity(),
    NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getView, QString("v"))),
    ObjectWithUid(uid),
    m_columNames(), m_canReplace(false), m_manual(manual)
{
    m_helper = new QueryGraphicsHelper();
    m_selectQuery = new SelectQuery(m_helper, 0, this);
    QueryAsGenerator::instance().initNewQuery(m_selectQuery);
    m_helper->setQuery(m_selectQuery);
}

View::View(Version*v, bool manual, QString uid) : SqlSourceEntity(), NamedItem(NameGenerator::getUniqueName(v, (itemGetter)&Version::getView, QString("v"))), ObjectWithUid(uid),
    m_columNames(), m_canReplace(false), m_manual(manual)
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
    viewElement.setAttribute("uid", getObjectUid());
    viewElement.setAttribute("class-uid", getClassUid());

    if(m_manual)
    {
        QDomElement textElement = doc.createElement("Sql");
        QDomCDATASection cdata = doc.createCDATASection(m_sql);
        textElement.appendChild(cdata);
        viewElement.appendChild(textElement);
    }
    else
    {
        viewElement.setAttribute("CanReplace", m_canReplace);
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
