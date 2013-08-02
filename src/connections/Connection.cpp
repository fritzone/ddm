#include "Connection.h"
#include "IconFactory.h"
#include "db_DatabaseEngine.h"
#include "strings.h"

Connection::Connection(const QString &name, bool autoConnect) : SerializableElement(), IssueOriginator(), NamedItem(name),
    TreeItem(), m_autoConnect(autoConnect), m_engine(0), m_state(DID_NOT_TRY), m_lastError("")
{

}

bool Connection::tryConnect()
{
    m_lastError = "";
    if(!m_engine)
    {
        m_state = FAILED;
        return false;
    }
    if(!m_engine->tryConnect(this))
    {
        m_state = FAILED;
        m_lastError = m_engine->getLastError();
        if(getLocation())
        {
            getLocation()->setIcon(0, IconFactory::getUnConnectedDatabaseIcon());
        }
        return false;
    }
    m_state = CONNECTED;
    return true;
}

QSqlDatabase Connection::getQSqlDatabase()
{
    return m_engine->getQSqlDatabaseForConnection(this);
}
