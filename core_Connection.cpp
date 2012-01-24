#include "core_Connection.h"
#include "db_DatabaseEngine.h"
#include "IconFactory.h"

Connection::Connection(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, bool savePw, bool autoConnect):
        SerializableElement(), TreeItem(), IssueOriginator(name),
        m_host(host), m_user(user), m_pass(pass), m_db(db), m_dbType("MySQL"), m_savePw(savePw), m_autoConnect(autoConnect), m_engine(0), m_state(DID_NOT_TRY)
{
    m_engine = DatabaseEngine::createEngine(m_dbType);
}

void Connection::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement connectionElement = doc.createElement("Connection");
    connectionElement.setAttribute("Name", getName());
    connectionElement.setAttribute("Host", m_host);
    connectionElement.setAttribute("Pass", m_pass);
    connectionElement.setAttribute("DB", m_db);
    connectionElement.setAttribute("User", m_user);
    connectionElement.setAttribute("DbType", m_dbType);
    connectionElement.setAttribute("AutoConnect", m_autoConnect);
    connectionElement.setAttribute("LastState", m_state);
    parent.appendChild(connectionElement);
}

bool Connection::tryConnect()
{
    if(!m_engine->tryConnect(this))
    {
        m_state = FAILED;
        if(getLocation())
        {
            getLocation()->setIcon(0, IconFactory::getUnConnectedDatabaseIcon());
        }
        return false;
    }
    m_state = CONNECTED;
    return true;
}

void Connection::resetTo(const QString &name, const QString &host, const QString &user, const QString &pass, const QString &db, bool savePw, bool autoConnect)
{
    setName(name);
    m_host = host;
    m_user = user;
    m_pass = pass;
    m_db = db;
    m_dbType = "MySQL";
    m_savePw = savePw;
    m_autoConnect = autoConnect;
    tryConnect();
}

QSqlDatabase Connection::getQSqlDatabase()
{
    return m_engine->getQSqlDatabaseForConnection(this);
}
