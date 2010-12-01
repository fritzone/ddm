#ifndef ISSUE_H
#define ISSUE_H

#include "NamedItem.h"
#include "TreeItem.h"

#include <QString>

class IssueOriginator;

/**
 * Class representing an identified issue
 */
class Issue : public NamedItem, public TreeItem
{
public:

    enum IssueType
    {
        WARNING=0,
        CRITICAL=1,
        INFO=2,
        RECOMMENDATION=3
    };

    Issue(IssueOriginator* originator, const QString& description, IssueType type, int nr) : NamedItem("issue #" + QString::number(nr)),
        m_originator(originator), m_description(description), m_type(type), m_id(nr)
    {}

    int getId() const
    {
        return m_id;
    }

    QString typeAsString() const
    {
        switch(m_type)
        {
        case WARNING:
            return "Warning";
        case CRITICAL:
            return "Critical";
        case INFO:
            return "Info";
        case RECOMMENDATION:
            return "Hint";
        }
    }

    IssueType getType() const
    {
        return m_type;
    }

    const IssueOriginator * getOriginator() const
    {
        return m_originator;
    }

    QString getDescription() const
    {
        return m_description;
    }

private:
    QString m_description;
    IssueType m_type;
    IssueOriginator* m_originator;
    int m_id;
};

#endif // ISSUE_H
