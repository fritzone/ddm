#ifndef ISSUEMANAGER_H
#define ISSUEMANAGER_H

#include "Issue.h"

#include <QDockWidget>

class Table;
class Column;

class IssueManager
{
public:

    static IssueManager& getInstance()
    {
        static IssueManager instance;
        return instance;
    }

    void setIssuesDock(QDockWidget* widget)
    {
        dw = widget;
    }

    QVector<Issue*> getIssuesOfTable(const QString& tabName)
    {
        if(issuesOfTables.contains(tabName))
        {
            return issuesOfTables[tabName];
        }
        return QVector<Issue*>();
    }

    void ignoringIssue(Issue*,bool alsoRemoveFromOriginator = true);

    Issue* createDatabaseNormalizationIssue( Table* newTab, Column* newCol, Table* firstTab, Column* firstCol);
    Issue* createForeignKeyReccomendedIssue( Table* newTab, Column* newCol, Table* firstTab, Column* firstCol);

    int nextCounter()
    {
        return ++issueCounter;
    }

private:

    IssueManager()
    {
        issueCounter = 0;
        dw = 0;
    }

    Issue* insertIssue(Issue* issue, IssueOriginator* originator, const QString& description, Issue::IssueSeverity type);


    bool tableAlreadyHasIssues(const Table*);

private:

    int issueCounter;
    QDockWidget* dw;

    // mapping a table name to the twitem it holds the issues in.
    QMap<QString, ContextMenuEnabledTreeWidgetItem*> originatorTables;

    // mapping table names to issues associated with it
    QMap<QString, QVector <Issue*> > issuesOfTables;

};

#endif // ISSUEMANAGER_H
