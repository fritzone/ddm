#include "core_ConnectionIssue.h"
#include "core_Connection.h"
#include "IssueManager.h"

ConnectionIssue::ConnectionIssue(Connection* connection, QString reason) : Issue(connection, reason, Issue::CRITICAL, IssueManager::getInstance().nextCounter()), m_connection(connection)
{
}

bool ConnectionIssue::stillValid()
{
    m_connection->tryConnect();
    if(m_connection->getState() == Connection::FAILED) return true;
    return false;
}
