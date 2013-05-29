#include "db_BasicSqlGenerator.h"
#include "core_Table.h"
#include "strings.h"
#include "SpInstance.h"
#include "ObjectWithSpInstances.h"
#include "db_DatabaseEngine.h"

QString BasicSqlGenerator::generateTableCreationComments(const Table *t, const QString& tabName) const
{
    QString comment;
    QString desc = t->getDescription();
    QStringList lines = desc.split(strNewline);
    if(lines.size())
    {
        for(int i=0; i< lines.size(); i++)
        {
            comment += strSqlComment +  lines.at(i) + strNewline;
        }
    }
    else
    {
        comment = strSqlComment
                  + (desc.length() > 0 ? desc: "Create table " + tabName)
                  + strNewline;
    }
    return comment;
}


QString BasicSqlGenerator::keyword(QString a) const
{
    QString result;
    if(m_upcase)
    {
        result = a.toUpper();
    }
    result = a.toLower();
    result += strSpace;

    return result;
}

QString BasicSqlGenerator::spiResult(ObjectWithSpInstances* object, QUuid uid) const
{
    SpInstance* spi = object->getInstanceForSqlRoleUid(m_engine, uid);
    if(spi)
    {
        QString temporary = spi->get();
        if(temporary == "TRUE")
        {
            QString kw = m_engine->spiExtension(uid);
            if(kw.isEmpty())
            {
                return "";
            }

            if(m_upcase)
            {
                kw = kw.toUpper();
            }
            else
            {
                kw = kw.toLower();
            }
            kw += " ";
            return kw;
        }
    }
    return "";
}
