#include "core_StoredMethod.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"
#include <QStringList>

void StoredMethod::setSql(const QString &s)
{
    m_sql = s;
}

QStringList StoredMethod::generateSqlSource(AbstractSqlGenerator * gen, QHash<QString, QString> /*opts*/, const Connection* dest)
{
    QStringList r;
    r.append(gen->generateCreateStoredMethodSql(this, Configuration::instance().sqlOpts()));
    return r;
}


void StoredMethod::rename(const QString &n)
{
    setName(n);
    TreeItem::rename(n);
}

QString StoredMethod::getSql() const
{
    return m_sql;
}

QVector<StoredMethod::ParameterAndDescription> StoredMethod::getParametersWithDescription()
{
    QVector<StoredMethod::ParameterAndDescription> result;
    QStringList lines = m_sql.split('\n');
    for(int i=0; i<lines.size(); i++)
    {
        int docKeywordIndex = lines.at(i).indexOf("@param");
        if(docKeywordIndex != -1)
        {
            docKeywordIndex += 7;

            while(! lines.at(i).at(docKeywordIndex).isLetterOrNumber() && lines.at(i).at(docKeywordIndex) != '_') docKeywordIndex ++;
            QString pName = "";
            while(lines.at(i).at(docKeywordIndex).isLetterOrNumber() || lines.at(i).at(docKeywordIndex) == '_')
            {
                pName += lines.at(i).at(docKeywordIndex);
                docKeywordIndex ++;
            }

            while(! lines.at(i).at(docKeywordIndex).isLetterOrNumber()) docKeywordIndex ++;
            QString pDesc = "";
            while(docKeywordIndex < lines.at(i).length())
            {
                pDesc += lines.at(i).at(docKeywordIndex);
                docKeywordIndex ++;
            }
            StoredMethod::ParameterAndDescription pad;
            pad.m_parameter = pName;
            pad.m_description = pDesc;
            result.push_back(pad);
        }
        else
        {
            docKeywordIndex = lines.at(i).indexOf("@brief");
            if(docKeywordIndex != -1)
            {
                 docKeywordIndex += 7;
                 QString mthBriefDesc = "";
                 while(docKeywordIndex < lines.at(i).length())
                 {
                     mthBriefDesc += lines.at(i).at(docKeywordIndex);
                     docKeywordIndex ++;
                 }
                 m_brief = mthBriefDesc;
            }
        }
    }
    return result;
}
