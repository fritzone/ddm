#ifndef ISSUEORIGINATOR_H
#define ISSUEORIGINATOR_H

#include "NamedItem.h"

#include <QVector>

class Table;
class Issue;

/**
 * Basic abstract class for all the classes that can originate issues...
 */
class IssueOriginator
{
public:

    IssueOriginator();

    virtual ~IssueOriginator() {}

    virtual QString getFullLocation() const = 0;

    virtual Table* getIssueTable() const = 0;

    void addIssue(Issue* issue)
    {
        m_issues.append(issue);
    }

    const QVector<Issue*>& getIssues() const
    {
        return m_issues;
    }

    /// this removes only from the internal vector
    void removeIssueByName(const QString& name, bool removeFromManagerToo = true);

    bool hasIssueLike(Issue* issue);

    void ignoreIssue(Issue* issue)
    {
        m_ignoredIssues.append(issue);
    }

private:

    QVector<Issue*> m_issues;

    QVector<Issue*> m_ignoredIssues;

};

#endif // ISSUEORIGINATOR_H
