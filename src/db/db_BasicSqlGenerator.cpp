#include "db_BasicSqlGenerator.h"
#include "core_Table.h"
#include "strings.h"
#include "SpInstance.h"
#include "ObjectWithSpInstances.h"
#include "db_DatabaseEngine.h"

QString BasicSqlGenerator::generateTableCreationComments(const Table *t, const QString& tabName) const
{
    if(!m_comments)
    {
        return "";
    }

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


QString BasicSqlGenerator::correctCase(QString a) const
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

QString BasicSqlGenerator::spiResult(const ObjectWithSpInstances *object, QUuid uid) const
{
    SpInstance* spi = object->getInstanceForSqlRoleUid(m_engine, uid);
    if(spi)
    {
        QString spsClassUid = spi->getClass()->getClassUid().toString() ;
        if(spsClassUid == uidTrueFalseSp)
        {
            QString temporary = spi->get();
            if(temporary == "TRUE")
            {
                QString kw = m_engine->spiExtension(uid);
                if(kw.isEmpty())
                {
                    return strSpace;
                }

                if(m_upcase)
                {
                    kw = kw.toUpper();
                }
                else
                {
                    kw = kw.toLower();
                }
                kw = strSpace + kw + strSpace;
                return kw;
            }
        }
        else
        if(spsClassUid == uidValueSp || spsClassUid == uidValueListSp)
        {
            QString temporary = spi->get();
            temporary = strSpace + temporary + strSpace;
            return temporary;

        }
    }
    return strSpace;
}

void BasicSqlGenerator::initForOptions(const QHash<QString, QString> &options) const
{
    m_upcase = Configuration::instance().sqlOptsGetUpcase(options);
    m_comments = options.contains(strGenerateComments) && options[strGenerateComments] == strYes;
    m_backticks = options.contains(strBackticks) && options[strBackticks] == strYes;
    m_pkpos = Configuration::instance().sqlOptsGetPkPosition(options);
    m_fkpos = Configuration::instance().sqlOptsGetFkPosition(options);    // the list of primary key columns, used only if pkpos is 1 or 2

}

QString BasicSqlGenerator::requestedTableName(const QString& tabName) const
{
    QString tableName = m_backticks ? strBacktick :strEmpty;
    tableName += tabName;
    tableName += m_backticks ? strBacktick : strEmpty;
    tableName += strNewline + strOpenParantheses + strNewline;
    return tableName;

}

QString BasicSqlGenerator::startCreateTableCommand(Table *table) const
{
    QString createTable = correctCase("create");
    createTable += spiResult(table, uidTemporaryTable);
    createTable += correctCase("table");

    return createTable;
}

QString BasicSqlGenerator::generateColumnList(Table* table) const
{
    QString columns;
    for(int i=0; i<table->fullColumns().size(); i++)
    {
        // column name
        Column *col = table->getColumn(table->fullColumns()[i]);
        if(col == 0)
        {
            col = table->getColumnFromParents(table->fullColumns()[i]);
            if(col == 0)
            {
                return "";
            }
        }

        columns += sqlForAColumn(col);
        // do we have more columns after this?
        if(i<table->fullColumns().size() - 1)
        {
            columns += strComma + strNewline;
        }
    }
    return columns;
}


QString BasicSqlGenerator::generatePrimaryKeys(const QStringList &primaryKeys) const
{
    QString pkCommand = strNewline + strTab + strComma ;
    pkCommand += correctCase("primary key");
    pkCommand += strOpenParantheses;
    for(int i=0; i<primaryKeys.size(); i++)
    {
        pkCommand += primaryKeys[i];
        if(i<primaryKeys.size() - 1)
        {
            pkCommand += strComma + strSpace;
        }
    }
    pkCommand += strCloseParantheses;
    return pkCommand;
}

QString BasicSqlGenerator::generateForeignKeys(const QStringList& foreignKeys) const
{
    QString fkCommand = "";
    for(int i=0; i<foreignKeys.size(); i++)
    {
        fkCommand += strSpace + foreignKeys[i];
        if(i<foreignKeys.size() - 1)
        {
            fkCommand += strComma + strNewline;
        }
    }

    return fkCommand;
}
