#include "IssueManager.h"
#include "IssueOriginator.h"
#include "Workspace.h"
#include "Table.h"
#include "Column.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "ContextMenuEnabledTreeWidget.h"
#include "DatabaseNormalizationIssue.h"
#include "ForeignKeyReccomendationIssue.h"

#include <QApplication>

Issue* IssueManager::insertIssue(Issue* issue, IssueOriginator *originator, const QString &description, Issue::IssueSeverity type)
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
    if(firstCol->hasIssueLike(newIssue))
    {
        delete newIssue;
        return 0;
    }
    Workspace::getInstance()->workingVersion()->addIssuse(newIssue);
    firstCol->addIssue(newIssue);

    insertIssue(newIssue, firstCol, newIssue->getDescription(), newIssue->getType());
    return newIssue;
}

Issue* IssueManager::createForeignKeyReccomendedIssue( Table* newTab, Column* newCol, Table* firstTab, Column* firstCol)
{
    ForeignKeyReccomendationIssue* newIssue = new ForeignKeyReccomendationIssue(newTab, newCol, firstTab, firstCol);
    if(firstCol->hasIssueLike(newIssue))
    {
        delete newIssue;
        return 0;
    }
    Workspace::getInstance()->workingVersion()->addIssuse(newIssue);
    firstCol->addIssue(newIssue);

    insertIssue(newIssue, firstCol, newIssue->getDescription(), newIssue->getType());
    return newIssue;
}
