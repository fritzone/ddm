#include "IssueOriginator.h"
#include "IssueManager.h"
#include "Workspace.h"
#include "Version.h"

void IssueOriginator::removeIssueByName(const QString& name, bool removeFromManagerToo)
{
    for(int i=0; i< m_issues.size(); i++)
    {
        if(m_issues[i]->getName() == name)
        {
            delete m_issues[i]->getLocation();
            if(removeFromManagerToo) IssueManager::getInstance().ignoringIssue(m_issues[i], false);
            m_issues.remove(i);
            return;
        }
    }
}

bool IssueOriginator::hasIssueLike(Issue *issue)
{
    for(int i=0; i< m_issues.size(); i++)
    {
        if(m_issues.at(i)->isLike(issue))
        {
            return true;
        }
    }
    return false;
}
