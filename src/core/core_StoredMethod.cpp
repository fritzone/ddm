#include "core_StoredMethod.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"
#include "ddm_strings.h"

#include <QStringList>
#include <QCryptographicHash>

StoredMethod::StoredMethod(const QString& name, const QString& uid, Version *v, bool guided) :
    NamedItem(name), SerializableElement(), CloneableElement(), VersionElement(uid, v),
    TreeItem(), SqlSourceTreeItem(),
    m_sql(), m_brief(), m_desc(), m_returns(), m_guidedCreation(guided),
    m_javaMappedMethod(false), m_javaClassName(), m_javaMethodName()
{}


void StoredMethod::setSql(const QString &s)
{
    m_sql = s;
}

QStringList StoredMethod::generateSqlSource(AbstractSqlGenerator * gen, QHash<QString, QString> /*opts*/, const Connection* /*dest*/)
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

void StoredMethod::serialize_parameters(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement parametersEl = doc.createElement("Parameters");
    parametersEl.setAttribute("count", m_guidedParameters.size());
    for(int i=0; i<m_guidedParameters.size(); i++)
    {
        QDomElement parameterEl = doc.createElement("Parameter");
        parameterEl.setAttribute("index", i);
        parameterEl.setAttribute("name", m_guidedParameters[i]->m_parameter);
        parameterEl.setAttribute("type", m_guidedParameters[i]->m_type);
        parameterEl.setAttribute("description", m_guidedParameters[i]->m_description);
        parameterEl.setAttribute("direction", m_guidedParameters[i]->m_direction);
        parameterEl.setAttribute("prog-lang-type", m_guidedParameters[i]->m_progLangType);

        parametersEl.appendChild(parameterEl);
    }
    parent.appendChild(parametersEl);

    // and now the description part
    QDomElement descsEl = doc.createElement("Descriptions");

    QDomElement descEl = doc.createElement("Description");
    QDomCDATASection cdDesc = doc.createCDATASection(m_desc);
    descEl.appendChild(cdDesc);
    descsEl.appendChild(descEl);

    descsEl.setAttribute("brief", m_brief);
    parent.appendChild(descsEl);
}

QString StoredMethod::getSql() const
{
    return m_sql;
}

QVector<ParameterAndDescription *> StoredMethod::getParametersWithDescription()
{
    if(m_guidedCreation)
    {
        return m_guidedParameters;
    }

    m_brief = "TODO: write the brief description of the method";
    m_desc = "";
    m_returns = "TODO: document the return value of the function";

    QVector<ParameterAndDescription*> result;
    QStringList lines = m_sql.split('\n');
    int nameidx = 0;
    QString tName = getNameFromSql(0, nameidx);
    int predictedIndex = 0;
    for(int i=0; i<lines.size(); i++)
    {
        predictedIndex += lines.at(i).length();
        if(predictedIndex > nameidx) break;
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
            ParameterAndDescription* pad = new ParameterAndDescription(pName, pDesc);
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
            else
            {
                docKeywordIndex = lines.at(i).indexOf("@return");
                if(docKeywordIndex != -1)
                {
                     docKeywordIndex += 8;
                     QString mthReturnType = "";
                     while(docKeywordIndex < lines.at(i).length())
                     {
                         mthReturnType += lines.at(i).at(docKeywordIndex);
                         docKeywordIndex ++;
                     }
                     m_returns = mthReturnType;
                }
                else
                {
                    docKeywordIndex = lines.at(i).indexOf("@desc");
                    if(docKeywordIndex != -1)
                    {
                         docKeywordIndex += 6;
                         QString mthDescription = "";
                         while(docKeywordIndex < lines.at(i).length())
                         {
                             mthDescription += lines.at(i).at(docKeywordIndex);
                             docKeywordIndex ++;
                         }
                         m_desc += mthDescription;
                    }
                    else
                    {
                        QString line = lines.at(i);
                        line = line.mid(line.indexOf("--") + 2);
                        m_desc += line;
                    }
                }
            }
        }
    }

    // now feed in the SQL type from the sql for each parameter
    if(tName == "UNNAMED") return result;   // hmm hmm
    nameidx += tName.length();
    while(nameidx < m_sql.length() && m_sql.at(nameidx).isSpace()) nameidx ++;
//    qDebug() << nameidx << " =>" << m_sql.at(nameidx) << "<";
    if(nameidx < m_sql.length()  && m_sql.at(nameidx) != '(') return result;
    nameidx ++;
    QString cpar = "";
    while(nameidx < m_sql.length())
    {
        if(m_sql.at(nameidx) != ',' && m_sql.at(nameidx) != ')')
        {
            if(m_sql.at(nameidx) == '(') // parantheses, such as VARCHAR(255), INT(10, 3)
            {
                bool done = false;
                int clevel = 0;
                while(nameidx < m_sql.length() && !done)
                {
                    cpar = cpar + m_sql.at(nameidx);
                    nameidx ++;
                    if(m_sql.at(nameidx) == '(') clevel ++;
                    if(m_sql.at(nameidx) == ')')
                    {
                        clevel --;
                        if(clevel == -1)
                        {
                            cpar = cpar + m_sql.at(nameidx);
                            done = true;
                        }
                    }
                }
            }
            else
            {
                cpar = cpar + m_sql.at(nameidx);
                nameidx ++;
            }
        }
        else
        {
            nameidx ++;
            QStringList pars = cpar.split(QRegExp("\\s+"));
            QString direction = "";
            QString pname = "";
            QString ptype = "";

            if(pars.length() == 3)
            {
                direction = pars[0];
                pname = pars[1];
                ptype = pars[2];
            }
            else
            if(pars.length() == 2)
            {
                pname = pars[0];
                ptype = pars[1];
            }
            else
            {
                return result;
            }

            bool found = false;
            for(int i=0; i< result.size(); i++)
            {
                if(result.at(i)->m_parameter.toUpper() == pname.toUpper())
                {
                    result[i]->m_direction = direction;
                    result[i]->m_type = ptype;
                    result[i]->m_source = VER_IN_BOTH;
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                ParameterAndDescription* pad = new ParameterAndDescription(pname,
                                            ptype, QObject::tr("TODO: Write proper documentation."), direction, "", PARAM_LIST);
                result.push_back(pad);
            }
        }
    }
    return result;
}


QString StoredMethod::getNameFromSql(int stidx, int &nameidx)
{
    QString t = m_sql;
    QString c = keyword();
    int i = t.indexOf(c, stidx, Qt::CaseInsensitive);
    nameidx = -1;

    if(i != -1)
    {
        // search backward to see if the previous word is "CREATE"
        int cindx = i-1;
        while(cindx >= 0 && t.at(cindx).isSpace()) cindx --;
        QString prev = "";
        while(cindx>-1 && !t.at(cindx).isSpace() && t.at(cindx)!='=')
        {
            prev = t.at(cindx) + prev;
            cindx --;
        }
        if(cindx <= 0)
        {
            return "UNNAMED";
        }
        if(prev.toUpper() != strCreate)
        {
            // search back if we have definer ...
            while(cindx >= 0 && t.at(cindx).isSpace()) cindx --;
            // this should be '='
            if(cindx >= 0 && t.at(cindx) == '=')
            {
                cindx --;
                // skip space before =
                while(cindx >= 0 && t.at(cindx).isSpace()) cindx --;
                // prev2 is supposed to be be DEFINER
                QString prev2 = "";
                while(cindx>-1 && !t.at(cindx).isSpace())
                {
                    prev2 = t.at(cindx) + prev2;
                    cindx --;
                }
                if(prev2.toUpper() == "DEFINER")
                {
                    // skip space before DEFINER
                    while(cindx >= 0 && t.at(cindx).isSpace()) cindx --;
                    // prev2 is supposed to be be CREATE
                    QString prev3 = "";
                    while(cindx>-1 && !t.at(cindx).isSpace())
                    {
                        prev3 = t.at(cindx) + prev3;
                        cindx --;
                    }
                    if(prev3.toUpper() != strCreate)
                    {
                        i += c.length();
                        return getNameFromSql(i, nameidx);
                    }
                }
                else
                {
                    i += c.length();
                    return getNameFromSql(i, nameidx);
                }
            }
            else
            {
                i += c.length();
                return getNameFromSql(i, nameidx);
            }
        }

        i += c.length();
        while(i<t.length() && t.at(i).isSpace())
        {
            i++;
        }
        QString n = "";
        nameidx = i;
        while(i<t.length() && t.at(i) != '(')
        {
            n += ((t.at(i).isSpace())?(QString("")):(t.at(i)));
            if(t.at(i).isSpace())   // error, space in the name declaration
            {
                nameidx = -1;
                return "UNNAMED";
            }
            i++;
        }
        return n;
    }

    return "UNNAMED";
}

bool StoredMethod::hasParameter(const QString &name)
{
    for(int i=0; i<m_guidedParameters.size(); i++)
    {
        if(m_guidedParameters[i]->m_parameter == name)
        {
            return true;
        }
    }
    return false;
}

ParameterAndDescription *StoredMethod::getParameter(const QString &name)
{
    for(int i=0; i<m_guidedParameters.size(); i++)
    {
        if(m_guidedParameters[i]->m_parameter == name)
        {
            return m_guidedParameters[i];
        }
    }
    return 0;
}

void StoredMethod::removeParameter(const QString &name)
{
    for(int i=0; i<m_guidedParameters.size(); i++)
    {
        if(m_guidedParameters.at(i)->m_parameter == name)
        {
            m_guidedParameters.remove(i);
            return;
        }
    }
}

void StoredMethod::moveDownParameter(int i)
{
    ParameterAndDescription* temp = m_guidedParameters[i + 1];
    m_guidedParameters[i + 1] = m_guidedParameters[i];
    m_guidedParameters[i] = temp;
}

void StoredMethod::moveUpParameter(int i)
{
    ParameterAndDescription* temp = m_guidedParameters[i - 1];
    m_guidedParameters[i - 1] = m_guidedParameters[i];
    m_guidedParameters[i] = temp;
}

QString StoredMethod::getSqlHash() const
{
    QString s = m_sql;
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
