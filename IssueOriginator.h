#ifndef ISSUEORIGINATOR_H
#define ISSUEORIGINATOR_H

#include "NamedItem.h"

/**
 * Basic abstract class for all the classes that can originate issues...
 */
class IssueOriginator : public NamedItem
{
public:
    IssueOriginator(const QString& name) : NamedItem(name) {}

    virtual QString getFullLocation() const = 0;
};

#endif // ISSUEORIGINATOR_H
