#include "IssueManager.h"
#include "Workspace.h"
#include "Version.h"
#include "VersionGuiElements.h"
#include "ContextMenuEnabledTreeWidget.h"

int IssueManager::issueCounter = 0;
QDockWidget* IssueManager::dw = 0;

Issue* IssueManager::createIssue(IssueOriginator *originator, const QString &description, Issue::IssueType type)
{
    Issue* newIssue = new Issue(originator, description, type, ++issueCounter);
    Workspace::getInstance()->workingVersion()->addIssuse(newIssue);

    if(dw)
    {
        dw->show();
        ContextMenuEnabledTreeWidgetItem* itm = Workspace::getInstance()->workingVersion()->getGui()->createIssueTreeEntry(newIssue);

    }

    return newIssue;
}
