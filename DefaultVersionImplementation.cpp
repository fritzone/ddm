#include "DefaultVersionImplementation.h"

#include "ContextMenuCollection.h"
#include "UserDataType.h"
#include "Table.h"
#include "Diagram.h"
#include "IconFactory.h"
#include "TableInstance.h"
#include "Project.h"
#include "ForeignKey.h"
#include "NameGenerator.h"
#include "VersionGuiElements.h"
#include "Workspace.h"
#include "Configuration.h"
#include "db_DatabaseEngine.h"
#include "Issue.h"
#include "IssueManager.h"
#include "core_View.h"
#include "core_Procedure.h"
#include "db_AbstractSQLGenerator.h"
#include "core_Trigger.h"
#include "core_Function.h"
#include "strings.h"

#include <QtGui>

DefaultVersionImplementation::DefaultVersionImplementation(Project* p, int major, int minor)
    : Version(major, minor),
      version(""), m_data(), m_project(p), m_guiElements(0), m_validationFlags(0)
{
}

void DefaultVersionImplementation::createTreeItems(GuiElements* gui, ContextMenuEnabledTreeWidgetItem* projectItem)
{
    m_guiElements = new VersionGuiElements(gui, this);
    m_guiElements->createGuiElements(projectItem);
}

void DefaultVersionImplementation::addNewDataType(UserDataType* dt)
{
    int i = 0;
    while(i < m_data.m_dataTypes.size())
    {
        if(m_data.m_dataTypes.at(i)->getType() != dt->getType()) i++;
        else break;
    }

    m_data.m_dataTypes.insert(i, dt);

}

bool DefaultVersionImplementation::hasDataType(const QString& name) const
{
    for(int i=0; i < m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == name)
        {
            return true;
        }
    }
    return false;
}

UserDataType* DefaultVersionImplementation::getDataType(const QString& name)
{
    for(int i=0; i< m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == name)
        {
            return const_cast<UserDataType*>(m_data.m_dataTypes.at(i));  // yeah, this sucks
        }
    }
    return 0;
}

int DefaultVersionImplementation::getDataTypeIndex(const QString& name)
{
    for(int i=0; i < m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == name)
        {
            return i;
        }
    }
    return -1;
}

inline void DefaultVersionImplementation::addTable(Table *t)
{
    m_data.m_tables.append(t);
}

inline void DefaultVersionImplementation::addDiagram(Diagram* d)
{
    m_data.m_diagrams.append(d);
}

inline bool DefaultVersionImplementation::hasTable(Table *t)
{
    return m_data.m_tables.indexOf(t) >= 0;
}

inline bool DefaultVersionImplementation::hasTable(const QString& tb)
{
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        if(QString::compare(m_data.m_tables[i]->getName(), tb, Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }
    return false;
}


// TODO: 1. Make the Table class to come from the named item
//       2. the two methods below could be very easily templated after this

Table* DefaultVersionImplementation::getTable(const QString &name) const
{
    for(int i=0; i< m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getName() == name)
        {
            return m_data.m_tables[i];
        }
    }
    return 0;
}

Diagram* DefaultVersionImplementation::getDiagram(const QString& name)
{
    for(int i=0; i<m_data.m_diagrams.size(); i++)
    {
        if(m_data.m_diagrams[i]->getName() == name)
        {
            return m_data.m_diagrams[i];
        }
    }

    return 0;
}

inline const QVector<UserDataType*>& DefaultVersionImplementation::getDataTypes() const
{
    return m_data.m_dataTypes;
}

inline const QVector<Table*>& DefaultVersionImplementation::getTables() const
{
    return m_data.m_tables;
}

void DefaultVersionImplementation::purgeSentencedTableInstances()
{
    int i=0;
    while(i<m_data.m_tableInstances.size())
    {
        if(m_data.m_tableInstances.at(i)->sentenced())
        {
            TableInstance* tinst = m_data.m_tableInstances.at(i);
            m_data.m_tableInstances.remove(i);
            delete tinst;
        }
        else
        {
            i++;
        }
    }
}

void DefaultVersionImplementation::deleteTableInstance(TableInstance *tinst)
{
    tinst->sentence();
    // now find which table instances were created because of this and remove them
    QVector<TableInstance*>& insted = tinst->getInstantiatedTableInstances();
    for(int i=0; i<insted.size(); i++)
    {
        // except if there are other table instances that have it as instantiated
        bool someoneElseAlsoInstantiatedThis = false;
        for(int j=0; j<m_data.m_tableInstances.size(); j++)
        {
            if(tinst != m_data.m_tableInstances.at(j) )
            {
                for(int k=0; k<m_data.m_tableInstances.at(j)->getInstantiatedTableInstances().size(); k++)
                {
                    if(m_data.m_tableInstances.at(j)->getInstantiatedTableInstances().at(k) == insted.at(i))
                    {
                        someoneElseAlsoInstantiatedThis = true;
                        break;
                    }
                }
                if(someoneElseAlsoInstantiatedThis)
                {
                    break;
                }
            }
        }
        if(!someoneElseAlsoInstantiatedThis)
        {
            if(!insted.at(i)->sentenced() && insted.at(i)->instantiatedBecuaseOfRkReference())
            {
                deleteTableInstance(insted.at(i));
            }
        }
    }
}

bool DefaultVersionImplementation::deleteTable(Table *tab)
{
    int tabIndex = -1;
    QString incomingForeignKeys = "";
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getForeignKeyToTable(tab->getName()) != 0)
        {
            incomingForeignKeys+= "\n - " + m_data.m_tables[i]->getName();
        }

        if(m_data.m_tables[i]->getName() == tab->getName())
        {
            tabIndex = i;
        }

    }
    if(tabIndex == -1) return false;
    if(incomingForeignKeys.length() > 0)
    {
        QMessageBox::warning(0, QObject::tr("Foreign keys found"),
                             QObject::tr("Cannot delete this table since the following tables are referencing it through a foreign key: ") +
                             incomingForeignKeys +
                             QObject::tr("\nFirstly remove the foreign keys, then delete the table."), QMessageBox::Ok);
        return false;
    }

    for(int i=0; i<m_data.m_diagrams.size(); i++)
    {
        m_data.m_diagrams[i]->removeTable(tab->getName());
    }

    // and now we should delete the table instances that were created absed on this table, and the SQLs too...
    int i=0;
    while(i<m_data.m_tableInstances.size())
    {
        if(m_data.m_tableInstances.at(i)->table()->getName() == tab->getName())
        {
            delete m_data.m_tableInstances.at(i)->getLocation();
            m_data.m_tableInstances.at(i)->onDelete();
            m_data.m_tableInstances.remove(i);
        }
        else
        {
            i++;
        }
    }


    // and delete the issues of other tables, this table was referenced in
    QVector<Issue*> issuesReferencingTheTable = IssueManager::getInstance().getIssuesReferencingTable(tab->getName());
    for(int i=0; i<issuesReferencingTheTable.size(); i++)
    {
        removeIssue(issuesReferencingTheTable .at(i)->getName());
        IssueManager::getInstance().ignoringIssue(issuesReferencingTheTable.at(i));
    }
    // now delete all the issues this table is referenced in
    QVector<Issue*> issuesOfTheTable = IssueManager::getInstance().getIssuesOfTable(tab->getName());
    for(int i=0; i<issuesOfTheTable.size(); i++)
    {
        removeIssue(issuesOfTheTable.at(i)->getName());
        IssueManager::getInstance().ignoringIssue(issuesOfTheTable.at(i));
    }
    getGui()->cleanupOrphanedIssueTableItems();

    // remove from the parenttable
    if(m_data.m_tables[tabIndex]->getParent())
    {
        m_data.m_tables[tabIndex]->getParent()->removeSpecializedTable(m_data.m_tables[tabIndex]);
    }

    ContextMenuEnabledTreeWidgetItem* tabLoc = tab->getLocation();
    delete tabLoc;
    m_data.m_tables.remove(tabIndex);
    return true;
}



void DefaultVersionImplementation::removeForeignKeyFromDiagrams(ForeignKey* fkToRemove)
{
    for(int i=0; i<m_data.m_diagrams.size(); i++)
    {
        m_data.m_diagrams[i]->forcefullyRemoveForeignKey(fkToRemove);
    }
}

Table* DefaultVersionImplementation::duplicateTable(Table *src)
{
    Table* dup = new Table(*src);
    dup->setName(NameGenerator::getUniqueName(this,  (itemGetter)&Version::getTable, src->getName()+"_copy"));
    addTable(dup);
    return dup;
}

void DefaultVersionImplementation::setupTableParentChildRelationships()
{
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getTempTabName().length() > 0)
        {
            m_data.m_tables[i]->setParent(getTable(m_data.m_tables[i]->getTempTabName()));
        }
    }
}

TableInstance* DefaultVersionImplementation::instantiateTable(Table* tab, bool becauseOfReference)
{
    TableInstance* tabInst = new TableInstance(tab, becauseOfReference, QUuid::createUuid().toString());
    TableInstance* other = getTableInstance(tabInst->getName());        // just to see that we have another instance of this table
    if(other && other != tabInst)
    {   // and in this case generate a new name for the current one
        tabInst->setName(NameGenerator::getUniqueName(this,  (itemGetter)&Version::getTableInstance, tabInst->getName()));
    }
    m_data.m_tableInstances.append(tabInst);
    return tabInst;
}

TableInstance* DefaultVersionImplementation::getTableInstance(const QString& name) const
{
    for(int i=0; i<m_data.m_tableInstances.size(); i++)
    {
        if(m_data.m_tableInstances.at(i)->getName() == name)
        {
            return m_data.m_tableInstances.at(i);
        }
    }
    return 0;
}

void DefaultVersionImplementation::deleteDataType(const QString& dtName)
{
    for(int i=0; i<m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == dtName)
        {
            m_data.m_dataTypes.remove(i);
            return;
        }
    }
}

UserDataType* DefaultVersionImplementation::duplicateDataType(const QString& name)
{
    UserDataType* src = 0;
    for(int i=0; i<m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == name)
        {
            src = m_data.m_dataTypes.at(i);
            break;
        }
    }
    if(!src)
    {
        return 0;
    }
    UserDataType* dt = new UserDataType(*src);
    dt->setName(NameGenerator::getUniqueName(this,  (itemGetter)&Version::getDataType, dt->getName()));
    addNewDataType(dt);
    return dt;
}

void DefaultVersionImplementation::deleteView(const QString &name)
{
    View* v = getView(name);
    m_data.m_views.remove(m_data.m_views.indexOf(v));
    delete v->getLocation();
    delete v->getSqlLocation();
    delete v;
}

void DefaultVersionImplementation::deleteProcedure(const QString& p)
{
    Procedure* v = getProcedure(p);
    m_data.m_procedures.remove(m_data.m_procedures.indexOf(v));
    delete v->getLocation();
    delete v->getSqlLocation();
    delete v;
}

void DefaultVersionImplementation::deleteDiagram(const QString& name)
{
    Diagram* dgr = getDiagram(name);
    m_data.m_diagrams.remove(m_data.m_diagrams.indexOf(dgr));
    delete dgr->getLocation();
    delete dgr;
}

QVector<Table*> DefaultVersionImplementation::getTablesReferencingAColumnThroughForeignKeys(const Column* col)
{
    QVector<Table*> result;
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        for(int j=0; j<m_data.m_tables.at(i)->getFks().size(); j++)
        {
            if(m_data.m_tables.at(i)->getFks().at(j)->foreignColumns().contains(col))
            {
                result.append(m_data.m_tables.at(i));
                break;
            }
        }
    }
    return result;
}

void DefaultVersionImplementation::setupForeignKeyRelationshipsForATable(Table* tab)
{
    for(int j=0; j<tab->getFks().size(); j++)
    {
        ForeignKey* fkJ = tab->getFks()[j];
        for(int k=0; k<fkJ->getAssociations().size(); k++)
        {
            ForeignKey::ColumnAssociation* assK = fkJ->getAssociation(k);
            // first: set the tables
            for(int l=0; l<getTables().size(); l++)
            {
                if(getTables().at(l)->getName() == assK->getSForeignTable())
                {
                    assK->setForeignTable(getTables().at(l));
                }
                if(getTables().at(l)->getName() == assK->getSLocalTable())
                {
                    assK->setLocalTable(getTables().at(l));
                }
            }
            // then: set the columns of those tables
            for(int l=0; l<assK->getLocalTable()->columns().size(); l++)
            {
                Column* colL = assK->getLocalTable()->getColumns().at(l);
                if(colL->getName() == assK->getSLocalColumn())
                {
                    assK->setLocalColumn(colL);
                }
            }
            for(int l=0; l<assK->getForeignTable()->columns().size(); l++)
            {
                Column* colL = assK->getForeignTable()->getColumns().at(l);
                if(colL->getName() == assK->getSForeignColumn())
                {
                    assK->setForeignColumn(colL);
                }
            }
        }
    }
}

QList<QString> DefaultVersionImplementation::getSqlScript(bool generateDelimiters)
{
    QList<QString> finalSql;
    QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
    opts["FKSposition"] = "OnlyInternal";
    opts["PKSposition"] = "AfterColumnsDeclaration";
    bool comments =  opts.contains("Comments") && opts["Comments"] == "Yes";

    if(Workspace::getInstance()->currentProjectIsOop())   // list the table instances' SQL
    {
        for(int i=0; i<getTableInstances().size(); i++)
        {
            QStringList sql = getTableInstances().at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts);
            finalSql << sql;
        }

        for(int i=0; i<getTableInstances().size(); i++)
        {
            QStringList foreignKeyCommands = m_project->getEngine()->getSqlGenerator()->generateAlterTableForForeignKeys(getTableInstances().at(i)->table(), opts);
            finalSql << foreignKeyCommands;
        }
    }
    else    // list table's SQL
    {
        for(int i=0; i<getTables().size(); i++)
        {
            QStringList sql = getTables().at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts);
            finalSql << sql;
        }

        for(int i=0; i<getTables().size(); i++)
        {
            QStringList foreignKeyCommands = m_project->getEngine()->getSqlGenerator()->generateAlterTableForForeignKeys(getTables().at(i), opts);
            finalSql << foreignKeyCommands;
        }
    }

    // and the views
    finalSql.append("\n");
    if(comments)
    {   finalSql << "-- Creating the views\n";
        finalSql.append("\n");
    }
    for(int i=0; i<m_data.m_views.size(); i++)
    {
        QStringList t = m_data.m_views.at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts);
        QString s = "";
        for(int j=0; j<t.size(); j++)
        {
            s += t.at(j) + " ";
        }
        s += ";\n\n";
        finalSql << s;
    }

    // and the procedures
    finalSql.append("\n");

    if(m_data.m_procedures.size() && comments)
    {
        finalSql << "-- Creating the procedures\n";
        finalSql.append("\n");
    }

    if(m_data.m_procedures.size() && generateDelimiters)
    {
        finalSql << m_project->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
    }

    for(int i=0; i<m_data.m_procedures.size(); i++)
    {
        QString s = "";
        if (comments) s = "-- Procedure " + m_data.m_procedures.at(i)->getName() + strNewline;
        QStringList t = m_data.m_procedures.at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts);
        for(int j=0; j<t.size(); j++)
        {
            s += t.at(j) + " ";
        }
        if(generateDelimiters)
        {
            s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
        }
        else
        {
            s += strSemicolon + strNewline;
        }
        finalSql << s;
    }


    if(m_data.m_procedures.size() && generateDelimiters)
    {
        finalSql << m_project->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
    }

    // and the functions
    finalSql.append("\n");
    if(m_data.m_functions.size() && generateDelimiters)
    {
        finalSql << m_project->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
    }

    if(m_data.m_functions.size() && comments)
    {
        finalSql << "-- Creating the functions";
        finalSql.append("\n");
    }

    for(int i=0; i<m_data.m_functions.size(); i++)
    {
        QString s = "";
        if (comments) s = "-- Function " + m_data.m_functions.at(i)->getName() + strNewline;
        QStringList t = m_data.m_functions.at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts);
        for(int j=0; j<t.size(); j++)
        {
            s += t.at(j) + " ";
        }
        if(generateDelimiters)
        {
            s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
        }
        else
        {
            s += strSemicolon + strNewline;
        }
        finalSql << s;
    }

    if(m_data.m_functions.size() && generateDelimiters)
    {
        finalSql << m_project->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
    }

    // and the triggers
    finalSql.append("\n");

    if(m_data.m_triggers.size() && comments)
    {
        finalSql << "-- Creating the triggers\n";
        finalSql.append("\n");
    }

    for(int i=0; i<m_data.m_triggers.size(); i++)
    {
        QString s = "";
        if (comments) s = "-- Trigger " + m_data.m_triggers.at(i)->getName() + strNewline;
        QStringList t = m_data.m_triggers.at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts);
        for(int j=0; j<t.size(); j++)
        {
            s += t.at(j) + " ";
        }
        finalSql << s;
    }

    return finalSql;
}

const QVector<Diagram*>& DefaultVersionImplementation::getDiagrams() const
{
    return m_data.m_diagrams;
}

const QVector<View*>& DefaultVersionImplementation::getViews()
{
    return m_data.m_views;
}

void DefaultVersionImplementation::addTableInstance(TableInstance* inst)
{
    m_data.m_tableInstances.append(inst);
}

const QVector<TableInstance*>& DefaultVersionImplementation::getTableInstances() const
{
    return m_data.m_tableInstances;
}

UserDataType* DefaultVersionImplementation::provideDatatypeForSqlType(const QString& name, const QString& sql, const QString& nullable, const QString& defaultValue, bool relaxed)
{
    QString type = sql;
    QString size = "";
    QString finalName = relaxed?
                        type + (size.length()>0?" ("+ size+")":"") + (defaultValue.length() > 0? ("=" + defaultValue) : "") + (nullable=="NO"?" Not Null":""):
                        name + (defaultValue.length() > 0? (" (Def: " + defaultValue) + ")" : "") + (nullable=="NO"?" Not Null":"");
    int stp = sql.indexOf('(') ;
    if(stp != -1)
    {
        type = sql.left(stp);
        size = sql.mid(stp + 1, sql.indexOf(')') - stp -1);
    }

    for(int i=0; i<m_data.m_dataTypes.size(); i++)
    {
        UserDataType* udt = m_data.m_dataTypes.at(i);
        if(QString::compare(udt->getSqlType(), type, Qt::CaseInsensitive) == 0)
        {
            if(udt->getSize() == size)
            {
                if((udt->isNullable() && QString::compare(nullable, "YES", Qt::CaseInsensitive) == 0) || (!udt->isNullable() && QString::compare(nullable, "NO", Qt::CaseInsensitive) == 0))
                {
                    if(udt->getDefaultValue() == defaultValue)
                    {
                        if(!relaxed)
                        {
                            if(udt->getName() == finalName)
                            {
                                return udt;
                            }
                        }
                        else
                        {
                            return udt;
                        }
                    }
                }
            }
        }
    }

    // nothing found, we should create a new data type with some default values
    UserDataType* newUdt = new UserDataType(finalName,
                                            Workspace::getInstance()->currentProjectsEngine()->getTypeStringForSqlType(type),
                                            type, size, defaultValue, "", QStringList(), false, type + " " + size,
                                            QString::compare(nullable, "YES", Qt::CaseInsensitive) == 0, false);

    newUdt->setName(NameGenerator::getUniqueName(this, (itemGetter)&Version::getDataType, newUdt->getName()));
    addNewDataType(newUdt);
    return newUdt;
}

QVector<Issue*> DefaultVersionImplementation::checkIssuesOfNewColumn(Column* inNewColumn, Table* inTable)
{
    QVector<Issue*> result;
    // firstly we need to check that the newly introduced columns' datatype is used in any other table
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        Table* tabI = m_data.m_tables.at(i);
        if(tabI != inTable)
        {
            QStringList fullColumns = tabI->fullColumns();
            for(int j=0; j<fullColumns.size(); j++)
            {
                const UserDataType* othersDataType = tabI->getDataTypeOfColumn(fullColumns.at(j));
                if(inNewColumn->getDataType()->getName() == othersDataType->getName())
                {
                    Column* otherColumn = tabI->getColumn(fullColumns.at(j));
                    if(otherColumn == 0) otherColumn = tabI->getColumnFromParents(fullColumns.at(j));
                    if(otherColumn == 0) continue;

                    // they share the same data type ... let's check if this data type is:
                    // 1. numeric and 2. primary key in one of them
                    if(othersDataType->getType() == DT_NUMERIC)
                    {
                        if(inNewColumn->isPk() || otherColumn->isPk()) // but ONLY if there is no foreign key between these two. TODO: check this too
                        {
                            if(m_validationFlags != 1)
                            {
                                Issue* issue = IssueManager::getInstance().createForeignKeyReccomendedIssue(tabI, otherColumn, inTable, inNewColumn);
                                if(issue)
                                {
                                    result.append(issue);
                                }
                            }
                        }
                    }
                    else
                    {
                        if(othersDataType->getType() != DT_DATETIME) // normalization does not matter for date/time
                        {
                            if(othersDataType->getSize().toInt() > 1)   // and for texts with at least 2 characters
                            {
                                Issue* issue = IssueManager::getInstance().createDatabaseNormalizationIssue(tabI, otherColumn, inTable, inNewColumn);
                                if(issue)
                                {
                                    result.append(issue);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

void DefaultVersionImplementation::addIssuse(Issue* issue)
{
    m_data.m_issues.append(issue);
}

Issue* DefaultVersionImplementation::getIssue(const QString& name)
{
    for(int i=0; i< m_data.m_issues.size(); i++)
    {
        if(m_data.m_issues[i]->getName() == name)
        {
            return m_data.m_issues[i];
        }
    }
    return 0;
}

void DefaultVersionImplementation::removeIssue(const QString& name)
{
    for(int i=0; i< m_data.m_issues.size(); i++)
    {
        if(m_data.m_issues[i]->getName() == name)
        {
            m_data.m_issues.remove(i);
            return;
        }
    }
}

QVector<Issue*>& DefaultVersionImplementation::getIssues()
{
    return m_data.m_issues;
}

void DefaultVersionImplementation::validateVersion(bool onRequest)
{
    if(!onRequest && !Configuration::instance().continuousValidation()) return;
    QVector<Issue*>& allIssues = getIssues();
    int i=0;
    while(i<allIssues.size())
    {
        if(!allIssues.at(i)->stillValid())
        {
            QString issueIName = allIssues.at(i)->getName();
            IssueOriginator* orig = allIssues.at(i)->getOriginator();
            removeIssue(issueIName);
            orig->removeIssueByName(issueIName);
            getGui()->cleanupOrphanedIssueTableItems();
        }
        else
        {
            i++;
        }
    }

    for(int tab_ctr = 0; tab_ctr<m_data.m_tables.size(); tab_ctr ++)
    {
        for(int i=0; i<m_data.m_tables.at(tab_ctr)->fullColumns().size(); i++)
        {
            Column* c = m_data.m_tables.at(tab_ctr)->getColumn(m_data.m_tables.at(tab_ctr)->fullColumns().at(i));
            if(c == 0) c = m_data.m_tables.at(tab_ctr)->getColumnFromParents(m_data.m_tables.at(tab_ctr)->fullColumns().at(i));
            checkIssuesOfNewColumn(c, m_data.m_tables.at(tab_ctr));
        }
    }
}

void DefaultVersionImplementation::setSpecialValidationFlags(int a)
{
    m_validationFlags = a;
}

View* DefaultVersionImplementation::getView(const QString &viewName) const
{
    for(int i=0; i< m_data.m_views.size(); i++)
    {
        if(m_data.m_views[i]->getName() == viewName)
        {
            return m_data.m_views[i];
        }
    }
    return 0;
}

void DefaultVersionImplementation::addView(View* v)
{
    m_data.m_views.append(v);
}

Trigger* DefaultVersionImplementation::getTrigger(const QString &triggerName) const
{
    for(int i=0; i< m_data.m_triggers.size(); i++)
    {
        if(m_data.m_triggers[i]->getName() == triggerName)
        {
            return m_data.m_triggers[i];
        }
    }
    return 0;
}

Function* DefaultVersionImplementation::getFunction(const QString &name) const
{
    for(int i=0; i< m_data.m_functions.size(); i++)
    {
        if(m_data.m_functions[i]->getName() == name)
        {
            return m_data.m_functions[i];
        }
    }
    return 0;
}

Procedure* DefaultVersionImplementation::getProcedure(const QString &procedureName) const
{
    for(int i=0; i< m_data.m_procedures.size(); i++)
    {
        if(m_data.m_procedures[i]->getName() == procedureName)
        {
            return m_data.m_procedures[i];
        }
    }
    return 0;
}

void DefaultVersionImplementation::addProcedure(Procedure* p)
{
    m_data.m_procedures.append(p);
}

void DefaultVersionImplementation::addFunction(Function* p)
{
    m_data.m_functions.append(p);
}

void DefaultVersionImplementation::addTrigger(Trigger* t)
{
    m_data.m_triggers.append(t);
}

const QVector<Procedure*>& DefaultVersionImplementation::getProcedures()
{
    return m_data.m_procedures;
}

const QVector<Function*>& DefaultVersionImplementation::getFunctions()
{
    return m_data.m_functions;
}

const QVector<Trigger*>& DefaultVersionImplementation::getTriggers()
{
    return m_data.m_triggers;
}

void DefaultVersionImplementation::cleanupDataTypes()
{
    QVector<int> sentencedIndices;
    for(int k=0; k<m_data.m_dataTypes.size(); k++)
    {
        QString dtName = m_data.m_dataTypes.at(k)->getName();
        const QVector<Table*>& allTables = getTables();
        QString usage = "";
        for(int i=0; i<allTables.size(); i++)
        {
            const QVector<Column*> & tablesColumns = allTables.at(i)->getColumns();
            for(int j=0; j<tablesColumns.size(); j++)
            {
                QString cn = tablesColumns.at(j)->getDataType()->getName();
                if(cn == dtName)
                {
                    usage += "Y";
                }
            }
        }

        if(usage.length() == 0)
        {
            sentencedIndices.prepend(k);
        }
    }

    for(int i=0; i<sentencedIndices.size(); i++)
    {
        QString dtName = m_data.m_dataTypes.at(sentencedIndices.at(i))->getName();
        delete m_data.m_dataTypes.at(sentencedIndices.at(i))->getLocation();
        deleteDataType(dtName);

    }
}

VersionGuiElements* DefaultVersionImplementation::getGui()
{
    return m_guiElements;
}

QString DefaultVersionImplementation::getVersionText()
{
    return version;
}

SqlSourceEntity* DefaultVersionImplementation::getSqlSourceEntityNamed(const QString &name) const
{
    SqlSourceEntity* ent = 0;
    if(m_project->oopProject())
    {
        ent = getTableInstance(name);
    }
    else
    {
        ent = getTable(name);
    }

    if(ent == 0)
    {
        ent = getView(name);
    }

    if(ent == 0)
    {
        ent = getProcedure(name);
    }

    if(ent == 0)
    {
        ent = getTrigger(name);
    }

    if(ent == 0)
    {
        ent = getFunction(name);
    }

    return ent;
}
