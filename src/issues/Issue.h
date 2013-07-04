#ifndef ISSUE_H
#define ISSUE_H

#include "core_NamedItem.h"
#include "TreeItem.h"

#include <QString>

class IssueOriginator;
class Table;

/**
 * Class representing an identified issue
 */
class Issue : public NamedItem, public TreeItem
{
public:

    enum IssueSeverity
    {
        WARNING=0,
        CRITICAL=1,         // Critical used only for the connection issues
        INFO=2,
        RECOMMENDATION=3
    };

    enum IssueType
    {
        RECOMMENDATION_SHOULD_CREATE_FOREIGN_KEY = 50
    };

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

        return "N/A";
    }

    IssueSeverity getType() const
    {
        return m_type;
    }

    IssueOriginator * getOriginator() const
    {
        return m_originator;
    }

    QString getDescription() const
    {
        return m_description;
    }

    Issue(IssueOriginator* originator, const QString& description, IssueSeverity type, int nr) : NamedItem("issue #" + QString::number(nr)),
        m_description(description), m_type(type), m_originator(originator), m_id(nr)
    {}

    virtual bool isLike(Issue*) = 0;

    virtual bool stillValid() = 0;

    /**
     * Return true if this issue affects the given table
     */
    virtual bool affects(Table*) = 0;

protected:
    QString m_description;

    IssueSeverity m_type;
    IssueOriginator* m_originator;
    int m_id;
};

#endif // ISSUE_H
