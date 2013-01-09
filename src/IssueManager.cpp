#include "IssueManager.h"
#include "IssueOriginator.h"
#include "Workspace.h"
#include "core_Table.h"
#include "core_Column.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "ContextMenuEnabledTreeWidget.h"
#include "DatabaseNormalizationIssue.h"
#include "ForeignKeyReccomendationIssue.h"
#include "ConnectionIssue.h"

#include <QApplication>

Issue* IssueManager::insertIssue(Issue* issue, IssueOriginator *originator, const QString &, Issue::IssueSeverity)
{
    if(dw)
    {
        dw->show();

        if(originator->getIssueTable())
        {
            ContextMenuEnabledTreeWidgetItem* tabitm = 0;
            if(tableAlreadyHasIssues(originator->getIssueTable()))  // find and insert the new itm in there
            {
                tabitm = originatorTables[originator->getIssueTable()->getName()];
                issuesOfTables[originator->getIssueTable()->getName()].append(issue);
            }
            //issuesOfTables[originator->getIssueTable()->getName()].append(issue);
            else    // create a new location
            {
                tabitm = Workspace::getInstance()->workingVersion()->getGui()->createTableTreeEntryForIssue(originator->getIssueTable());
                originatorTables[originator->getIssueTable()->getName()] = tabitm;

                QVector<Issue*> newVec;
                newVec.append(issue);
                issuesOfTables[originator->getIssueTable()->getName()] = newVec;
            }

            Workspace::getInstance()->workingVersion()->getGui()->createIssueTreeEntry(issue, tabitm);
            tabitm->setExpanded(true);
        }
    }

    return issue;
}


bool IssueManager::tableAlreadyHasIssues(const Table* tab)
{
    for(int i=0; i<originatorTables.keys().size(); i++)
    {
        if(originatorTables.keys().at(i) == tab->getName())
        {
            return true;
        }
    }
    return false;
}

void IssueManager::ignoringIssue(Issue* issue, bool alsoRemoveFromOriginator)
{
    if(alsoRemoveFromOriginator) issue->getOriginator()->removeIssueByName(issue->getName(), false);

    Table* issueTable = issue->getOriginator()->getIssueTable();

    if(issueTable)
    {
        QString issueTableName = issueTable->getName();
        QVector<Issue*>& issues = issuesOfTables[issueTableName];
        int idx = issues.indexOf(issue);
        if(idx != -1)
        {
            issues.remove(idx);
            if(issues.isEmpty())
            {
                issuesOfTables.remove(issueTableName);
                originatorTables.remove(issueTableName);
            }
        }
    }
}

Issue* IssueManager::createDatabaseNormalizationIssue( Table* newTab, Column* newCol, Table* firstTab, Column* firstCol)
{
    DatabaseNormalizationIssue* newIssue = new DatabaseNormalizationIssue(newTab, newCol, firstTab, firstCol);
    if(firstCol->hasIssueLike(newIssue) || newCol->hasIssueLike(newIssue))
    {
        delete newIssue;
        return 0;
    }
    Workspace::getInstance()->workingVersion()->addIssuse(newIssue);
    firstCol->addIssue(newIssue);

    insertIssue(newIssue, firstCol, newIssue->getDescription(), newIssue->getType());
    return newIssue;
}

Issue* IssueManager::createConnectionIssue(Connection *c, QString why)
{
    ConnectionIssue* ciss = new ConnectionIssue(c, why);
    Workspace::getInstance()->workingVersion()->addIssuse(ciss);
    Workspace::getInstance()->workingVersion()->getGui()->createIssueTreeEntry(ciss, 0);
    return ciss;
}

Issue* IssueManager::createForeignKeyReccomendedIssue( Table* newTab, Column* newCol, Table* firstTab, Column* firstCol)
{
    ForeignKeyReccomendationIssue* newIssue = new ForeignKeyReccomendationIssue(newTab, newCol, firstTab, firstCol);
    if(firstCol->hasIssueLike(newIssue) || newCol->hasIssueLike(newIssue))
    {
        delete newIssue;
        return 0;
    }
    Workspace::getInstance()->workingVersion()->addIssuse(newIssue);
    firstCol->addIssue(newIssue);

    insertIssue(newIssue, firstCol, newIssue->getDescription(), newIssue->getType());
    return newIssue;
}

QVector<Issue*> IssueManager::getIssuesReferencingTable(const QString& tabName)
{
    QVector<Issue*> result;
    for(int i=0; i<issuesOfTables.keys().size(); i++)
    {
        if(issuesOfTables.keys().at(i) != tabName)
        {
            QVector<Issue*>& tabsIssues = issuesOfTables[issuesOfTables.keys().at(i)];
            for(int j=0; j<tabsIssues.size(); j++)
            {
                if(tabsIssues.at(j)->affects(Workspace::getInstance()->workingVersion()->getTable(tabName)))
                {
                    result.append(tabsIssues.at(j));
                }
            }
        }
    }
    return result;
}
