#ifndef ISSUEMANAGER_H
#define ISSUEMANAGER_H

#include "Issue.h"

#include <QDockWidget>

class IssueManager
{
public:

    static Issue* createIssue(IssueOriginator* originator, const QString& description, Issue::IssueType type);
    static void setIssuesDock(QDockWidget* widget)
    {
        dw = widget;
    }

private:

    static int issueCounter;
    static QDockWidget* dw;

};

#endif // ISSUEMANAGER_H
