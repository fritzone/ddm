#include "db_BasicSqlGenerator.h"
#include "core_Table.h"
#include "strings.h"
#include "SpInstance.h"
#include "ObjectWithSpInstances.h"
#include "db_DatabaseEngine.h"
#include "ForeignKey.h"
#include "Index.h"
#include "core_TableInstance.h"
#include "core_UserDataType.h"
#include "core_View.h"
#include "qbr_SelectQuery.h"
#include "core_Trigger.h"

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
            if(temporary == strTrue)
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

QString BasicSqlGenerator::backtickedName(const QString& name) const
{
    QString result = (m_backticks?"`":"") + name;
    result += m_backticks?"`":"";
    result += " ";
    return result;
}

QStringList BasicSqlGenerator::generateCreateTableSql(Table *table,
                                const QHash<QString, QString> &options,
                                const QString& tabName,
                                const QMap<QString, QString> &fkMappings,
                                const Connection* pdest) const
{
    // do not generate any code for a table which has no columns
    if(table->fullColumns().size() == 0)
    {
        return QStringList();
    }

    // initialize basic code generation options
    initForOptions(options);

    // this will be returned
    QStringList toReturn;

    // first comments
    if(m_comments)
    {
        toReturn << generateTableCreationComments(table, tabName);
    }

    // creating the FOREIGN KEY sql(s)...
    QStringList foreignKeys = foreignKeyParticipants(table, fkMappings);

    // then the CREATE TABLE script
    QString createTable = createTableOnlyScript(table, foreignKeys, tabName, pdest);
    toReturn << createTable;

    // then creating the comments for the indexes if required
    if(m_comments)
    {
        QString comment = table->fullIndices().size()>0?"\n-- Create the indexes for table " + tabName + "\n":"";
        toReturn << comment;
    }

    // now create the indexes of the table
    appendCreateIndexCommands(table, toReturn, tabName);

    // and check if we have foreign keys
    if(m_fkpos == Configuration::AfterTable && foreignKeys.size() > 0)
    {
        if(m_comments)
        {
            QString comment = "-- Create the foreign keys for table " + tabName + "\n";
            toReturn << comment;
        }

        QString fkCommand = strNewline + correctCase("alter table") + tabName + correctCase("add");
        fkCommand += generateForeignKeys(foreignKeys);
        toReturn << fkCommand;
    }

    return toReturn;
}


QStringList BasicSqlGenerator::foreignKeyParticipants(Table *table, const QMap<QString, QString> &fkMappings) const
{
    QStringList foreignKeys;
    for(int i=0; i<table->getForeignKeys().size(); i++)
    {
        // just pre-render the SQL for foreign keys
        QString foreignKeysLocal = "";
        QString foreignKeysOther = "";

        ForeignKey* fkI = table->getForeignKeys().at(i);
        // the list of foreign key SQLs
        QString foreignKeysTable = fkI->getForeignTableName();
        if(fkMappings.contains(fkI->getName()))
        {
            QString fkTabInst = fkMappings[fkI->getName()];
            if(!fkTabInst.isEmpty())
            {
                foreignKeysTable = fkTabInst;
            }
        }
        for(int j=0; j<fkI->getAssociations().size(); j++)
        {

            ForeignKey::ColumnAssociation* assocJ = fkI->getAssociations().at(j);
            foreignKeysLocal += assocJ->getLocalColumn()->getName();
            foreignKeysOther += assocJ->getForeignColumn()->getName();

            if(j < fkI->getAssociations().size() - 1)
            {
                foreignKeysLocal += strComma + strSpace;
                foreignKeysOther += strComma + strSpace;
            }
        }

        QString foreignKeySql = strSpace + correctCase("constraint");
        foreignKeySql += fkI->getName() + strSpace;
        foreignKeySql += correctCase("foreign key") + strOpenParantheses;
        foreignKeySql += foreignKeysLocal + strCloseParantheses;
        foreignKeySql += correctCase("references");
        foreignKeySql += foreignKeysTable;
        foreignKeySql += strOpenParantheses + foreignKeysOther + strCloseParantheses;
        QString t = fkI->getOnDelete();
        if(t.length() > 0)
        {
            foreignKeySql += strSpace + correctCase("on delete") +correctCase(t);
        }
        t = fkI->getOnUpdate();
        if(t.length() > 0)
        {
            foreignKeySql += strSpace + correctCase("on update") + correctCase(t);
        }

        foreignKeys.append(foreignKeySql);

        // and send it back to the table for the final SQL rendering
        table->addForeignKeyCommand(foreignKeySql);
    }
    return foreignKeys;
}

void BasicSqlGenerator::appendCreateIndexCommands(Table* table, QStringList& toReturn, const QString& tabName) const
{
    for(int i=0; i<table->fullIndices().size(); i++)
    {
        Index* idx = table->getIndex(table->fullIndices().at(i));
        if(idx == 0)
        {
            idx = table->getIndexFromParents(table->fullIndices().at(i));
            if(idx == 0)
            {
                throw QString("ERROR: an index was not identified");
            }
        }

        QString indexCommand = correctCase("create");
        indexCommand += spiResult(idx, uidMysqlIndexCategory);
        indexCommand += correctCase("index");
        indexCommand += table->fullIndices().at(i);

        indexCommand += indexTypeSpecified(idx);

        // and the table
        indexCommand += correctCase("on") + tabName + strOpenParantheses;

        for(int j=0; j<idx->getColumns().size(); j++)
        {
            indexCommand += idx->getColumns().at(j)->getName();
            indexCommand += getIndexUsedLength(idx, idx->getColumns().at(j));
            indexCommand += idx->getOrderForColumn(idx->getColumns().at(j));
            if(j<idx->getColumns().size() - 1)
            {
                indexCommand += strComma + strSpace;
            }
        }
        indexCommand += strCloseParantheses + strSemicolon + strNewline;
        toReturn << indexCommand;
    }
}


QString BasicSqlGenerator::quotelessString(const QString& in) const
{
    QString result = "";
    for(int i=0; i<in.length(); i++)
    {
        if(in.at(i) != '"') result += in.at(i);
        else result += "\\\"";
    }
    return result;
}

QStringList BasicSqlGenerator::generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const
{
    initForOptions(options);
    QStringList result;
    int maxValues = 0;

    // find how many values we have, funny loop.
    for(int i=0; i<tableInstance->columns().size(); i++)
    {
        maxValues = tableInstance->values()[tableInstance->columns().at(i)].size();
        break;
    }

    Table* t = tableInstance->table();
    for(int i=0; i<maxValues; i++)
    {
        QString insert = strNewline + correctCase("insert into");
        insert += tableInstance->getName();
        insert += " (";
        for(int j=0; j<tableInstance->columns().size(); j++)
        {
            Column* c = t->getColumn(tableInstance->columns().at(j));
            if(!c) c = t->getColumnFromParents(tableInstance->columns().at(j));
            if(!c)
            {
                return QStringList();
            }

            bool ainc = false;
            {
                SpInstance* spi = c->getInstanceForSqlRoleUid(m_engine, uidColumnAutoIncrement);
                if(spi && spi->get() == strTrue )
                {
                    ainc = true;
                }
            }
            if(!ainc)
            {
                insert += tableInstance->columns().at(j);
                if(j< tableInstance->columns().size() - 1)
                {
                    insert += ", ";
                }
            }

        }

        insert += correctCase(") values (");
        for(int j=0; j<tableInstance->columns().size(); j++)
        {
            QVector<QString> vals = tableInstance->values()[tableInstance->columns().at(j)];
            Column* c = t->getColumn(tableInstance->columns().at(j));
            if(!c) c = t->getColumnFromParents(tableInstance->columns().at(j));
            if(!c)
            {
                return QStringList();
            }
            bool ainc = false;
            {
                SpInstance* spi = c->getInstanceForSqlRoleUid(m_engine, uidColumnAutoIncrement);
                if(spi && spi->get() == strTrue)
                {
                    ainc = true;
                }
            }
            if(!ainc)
            {
                QString val = vals.at(i);
                if(val.toUpper() != "NULL")
                {
                    if(val.length() > 0)
                    {
                        insert += "\"";
                        insert += quotelessString(vals.at(i));
                        insert += "\"";
                    }
                    else
                    {
                        // find the datatype, add the default value here
                        Column* pcol = tableInstance->table()->getColumn(tableInstance->columns().at(j));
                        if(pcol == 0) pcol = tableInstance->table()->getColumnFromParents(tableInstance->columns().at(j));
                        const UserDataType* udt = pcol->getDataType();
                        insert += "\"";
                        insert += udt->getDefaultValue();
                        insert += "\"";
                    }
                }
                else
                {
                    insert += correctCase("null");
                }
                if(j< tableInstance->columns().size() - 1)
                {
                    insert += ", ";
                }
            }
        }
        insert += ");\n";
        result << insert;
    }
    return result;
}


QStringList BasicSqlGenerator::generateDefaultValuesSql(Table* table, const QHash<QString, QString>& options) const
{
    QStringList result;
    initForOptions(options);
    const QVector <QVector <QString> >& sv = table->getDefaultValues();
    for(int i=0; i<sv.size(); i++)
    {
        const QVector<QString> &rowI = sv[i];
        QString insert = correctCase("insert into");
        insert += table->getName();
        insert += strOpenParantheses;
        for(int j=0; j<rowI.size(); j++)
        {
            insert += table->getColumns().at(j)->getName();
            if(j< rowI.size() - 1)
            {
                insert += strComma + strSpace;
            }
        }

        insert += strCloseParantheses + strSpace + correctCase("values")
                + strOpenParantheses;

        for(int j=0; j<rowI.size(); j++)
        {
            QString val = rowI[j];
            if(val.toUpper() != "NULL")
            {
                if(val.length() > 0)
                {
                    insert += "\"";
                    insert += quotelessString(rowI[j]);
                    insert += "\"";
                }
                else
                {
                    // find the datatype, add the default value here
                    Column* pcol = table->getColumns().at(j);
                    const UserDataType* udt = pcol->getDataType();
                    insert += "\"";
                    insert += udt->getDefaultValue();
                    insert += "\"";
                }
            }
            else
            {
                insert += correctCase("null");
            }
            if(j< rowI.size() - 1)
            {
                insert += strComma + strSpace ;
            }
        }

        insert += strCloseParantheses + strSemicolon + strNewline;
        result << insert;
    }
    return result;
}

QStringList BasicSqlGenerator::generateCreateViewSql(View *v, const QHash<QString, QString> &options) const
{
    initForOptions(options);
    if(v->isManual())
    {
        QStringList res;
        res.append(v->getManualSql());
        return res;
    }
    else
    {
        QStringList res;

        QString createViewSql = correctCase("create");

        createViewSql += createViewReplaceability(v);

        createViewSql += correctCase("view") + v->getName() + strSpace;

        // MySql supports column names, SQLITE does not
        createViewSql += createViewColumnNames(v);

        createViewSql += strSpace + correctCase("as") + strNewline;

        createViewSql += v->getQuery()->get();

        res.append(createViewSql);
        return res;
    }
}

QStringList BasicSqlGenerator::generateTriggerSql(Trigger* t, const QHash<QString, QString>& options) const
{
    initForOptions(options);
    QStringList result;
    QString s = correctCase("create trigger");
    s += t->getName() + strSpace;
    s += correctCase(t->getTime()) + strSpace;
    s += correctCase(t->getEvent()) + strSpace;
    s += correctCase("on");
    s += t->getTable() + strSpace;
    s += correctCase("for each row");
    s += strNewline + t->getSql() + strSemicolon + strNewline;
    result << s;
    return result;
}

QString BasicSqlGenerator::basicCreateTableScript(Table* table, const QStringList& foreignKeys, const QString& tabName) const
{
    QString createTable = startCreateTableCommand(table);

    // now something mysql specific
    createTable += spiResult(table, uidIfDoesNotExistTable);

    // table name
    createTable += requestedTableName(tabName);

    // creating the columns
    createTable += generateColumnList(table);

    QStringList primaryKeys = table->primaryKeyColumnsAsStringlist();
    if(primaryKeys.size() > 1) // if there is more than one we force it to be AfterColumns
    {
        m_pkpos = Configuration::PkAfterColumnsDeclaration;
    }

    // are we having primary keys after columns?
    if(m_pkpos == Configuration::PkAfterColumnsDeclaration && !primaryKeys.empty())
    {
        createTable += generatePrimaryKeys(primaryKeys);
    }

    // now check the foreign keys if any
    if(m_fkpos == Configuration::InTable && foreignKeys.size() > 0)
    {
        createTable += strComma + strNewline + generateForeignKeys(foreignKeys);
    }

    // this is closing the create table SQL
    createTable += strNewline + strCloseParantheses + strNewline;

    // and here we are done with the create table command
    return createTable;

}
