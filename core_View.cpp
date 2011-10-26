#include "core_View.h"
#include "qbr_QueryAsGenerator.h"
#include "qbr_SelectQuery.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"

View::View(bool manual) : SqlSourceEntity(), NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (NameGenerator::itemGetter)&Version::getView, QString("v"))),
                m_columNames(), m_canReplace(false), algorithm(), m_manual(manual)
{
    m_helper = new QueryGraphicsHelper();
    m_selectQuery = new SelectQuery(m_helper, 0, this);
    QueryAsGenerator::instance().initNewQuery(m_selectQuery);
    m_helper->setQuery(m_selectQuery);
}

QStringList View::generateSqlSource(AbstractSqlGenerator *, QHash<QString, QString>, const QString &codepage)
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
    return res;
}
