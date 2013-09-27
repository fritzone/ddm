#include "db_RepositoryQuery.h"
#include "Connection.h"

#include <QDebug>

RepositoryQuery::RepositoryQuery(const QDomElement& el) :
    m_id(el.attribute("id")),
    m_resultIndex(el.attribute("result_index").toInt())
{

    for(int i=0; i<el.childNodes().size(); i++)
    {
        QDomElement partI = el.childNodes().at(i).toElement();
        int partIndex = partI.attribute("index").toInt();
        QString typeI = partI.attribute("type");
        if(typeI.toLower() == "reference")
        {
            m_parts[partIndex] = partI.attribute("referred");
        }
        else
        {
            m_parts[partIndex] = partI.firstChild().toCDATASection().data();
        }
    }
}

QString RepositoryQuery::render(Connection* c)
{
    QList<int> indexes = m_parts.keys();
    qSort(indexes.begin(), indexes.end());
    QString result = "";
    for(int i=0; i<indexes.size(); i++)
    {
        QString t = m_parts[indexes[i]];
        if(t.startsWith("@"))
        {
            if(t.mid(1).toUpper() == "DB_NAME")
            {
                result += c->getDb();
            }
        }
        else
        {
            result += t;
        }
    }

    return result;
}
