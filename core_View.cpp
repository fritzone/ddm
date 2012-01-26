#include "core_View.h"
#include "qbr_QueryAsGenerator.h"
#include "qbr_SelectQuery.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"

View::View(bool manual) : SqlSourceEntity(), NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getView, QString("v"))),
                m_columNames(), m_canReplace(false), m_manual(manual)
{
    m_helper = new QueryGraphicsHelper();
    m_selectQuery = new SelectQuery(m_helper, 0, this);
    QueryAsGenerator::instance().initNewQuery(m_selectQuery);
    m_helper->setQuery(m_selectQuery);
}

View::View(Version*v, bool manual) : SqlSourceEntity(), NamedItem(NameGenerator::getUniqueName(v, (itemGetter)&Version::getView, QString("v"))),
                m_columNames(), m_canReplace(false), m_manual(manual)
{
    m_helper = new QueryGraphicsHelper();
}

void View::finalizeViewDeserialization()
{
    QueryAsGenerator::instance().initNewQuery(m_selectQuery);
}

QStringList View::generateSqlSource(AbstractSqlGenerator *, QHash<QString, QString>, const QString &/*codepage*/)
{
    if(m_manual)
    {
        QStringList res;
        res.append(m_sql);
        return res;
    }
    else
    {
        QStringList res;
        res.append("CREATE ");
        if(m_canReplace)
        {
            res.append("OR REPLACE ");
        }
        res.append(QString("VIEW ") + getName());
        if(m_columNames.size() > 0)
        {
            res.append(" (");
            QString c = "";
            for(int i=0; i<m_columNames.size(); i++)
            {
                c += m_columNames.at(i);
                if(i<m_columNames.size() - 1) c += ", ";
            }
            res.append(c);
            res.append(")");
        }
        res.append("\n");
        res.append("AS\n");
        res.append(m_selectQuery->get());
        QString g;
        for(int i=0; i< res.size(); i++)
        {
            g += res.at(i) + " ";
        }
        res.clear();
        res.append(g);
        return res;
    }
}

void View::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement viewElement = doc.createElement("View");
    viewElement.setAttribute("Name", m_name);
    viewElement.setAttribute("Manual", m_manual);

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
