#ifndef CORE_CONNECTIONISSUE_H
#define CORE_CONNECTIONISSUE_H

#include "Issue.h"

class Connection;

class ConnectionIssue : public Issue
{
public:
    ConnectionIssue(Connection* connection, QString reason);
    virtual bool isLike(Issue*)
    {
        return false;
    }

    virtual bool stillValid();

    /**
     * Return true if this issue affects the given table
     */
    virtual bool affects(Table*)
    {
        return false;
    }

private:
    Connection* m_connection;
};


#endif // CORE_CONNECTIONISSUE_H
