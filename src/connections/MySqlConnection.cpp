#include "MySqlConnection.h"
#include "db_DatabaseEngine.h"
#include "strings.h"
#include "db_DatabaseEngineManager.h"

MySqlConnection::MySqlConnection(const QString& name,
                                 const QString& host, const QString& user, const QString& pass, const QString& db,
                                 bool savePw, bool autoConnect, int port):
    Connection(name, autoConnect),
    m_host(host), m_user(user), m_pass(pass), m_db(db),  m_port(port), m_savePw(savePw)
{
    m_engine = DatabaseEngineManager::instance().engine(strMySql);
}

void MySqlConnection::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement connectionElement = doc.createElement("Connection");
    connectionElement.setAttribute("Name", getName());
    connectionElement.setAttribute("Host", m_host);
    connectionElement.setAttribute("Pass", m_pass);
    connectionElement.setAttribute("Port", m_port);
    connectionElement.setAttribute("DB", m_db);
    connectionElement.setAttribute("User", m_user);
    connectionElement.setAttribute("DbType", strMySql);
    connectionElement.setAttribute("AutoConnect", m_autoConnect);
    connectionElement.setAttribute("LastState", m_state);
    parent.appendChild(connectionElement);
}

void MySqlConnection::resetTo(const QString &name, const QString &host, const QString &user, const QString &pass, const QString &db, int port, bool savePw, bool autoConnect)
{
    setName(name);
    m_host = host;
    m_user = user;
    m_pass = pass;
    m_db = db;
    m_savePw = savePw;
    m_autoConnect = autoConnect;
    m_port = port;
    tryConnect();
}

void MySqlConnection::saveIntoSettings(QSettings &s)
{
    s.setValue(strName, getName());
    s.setValue(strHost, getHost());
    s.setValue(strPass, getPassword());
    s.setValue(strUser, getUser());
    s.setValue(strDB, getDb());
    s.setValue(strPort, QString::number(getPort()));
}
