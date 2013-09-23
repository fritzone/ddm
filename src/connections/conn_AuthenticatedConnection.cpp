#include "conn_AuthenticatedConnection.h"

#include "strings.h"

AuthenticatedConnection::AuthenticatedConnection(const QString &name,
                                                 const QString &host,
                                                 int port,
                                                 const QString &db,
                                                 const QString &user,
                                                 const QString &pass,
                                                 bool savePw,
                                                 bool autoConnect) :
    Connection(name, autoConnect),
    m_host(host), m_port(port), m_db(db), m_user(user), m_pass(pass),
    m_savePw(savePw)

{
}

void AuthenticatedConnection::resetTo(const QString &name,
                              const QString &host,
                              const QString &user,
                              const QString &pass,
                              const QString &db,
                              int port,
                              bool savePw,
                              bool autoConnect)
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

void AuthenticatedConnection::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement connectionElement = doc.createElement("Connection");
    connectionElement.setAttribute("Name", getName());
    connectionElement.setAttribute("Host", getHost());
    connectionElement.setAttribute("Pass", getPassword());
    connectionElement.setAttribute("Port", getPort());
    connectionElement.setAttribute("DB",   getDb());
    connectionElement.setAttribute("User", getUser());
    connectionElement.setAttribute("DbType", getDbType().toUpper());
    connectionElement.setAttribute("AutoConnect", m_autoConnect);
    connectionElement.setAttribute("LastState", m_state);
    parent.appendChild(connectionElement);
}

void AuthenticatedConnection::saveIntoSettings(QSettings &s)
{
    s.setValue(strName, getName());
    s.setValue(strHost, getHost());
    s.setValue(strPass, getPassword());
    s.setValue(strUser, getUser());
    s.setValue(strDB, getDb());
    s.setValue(strPort, QString::number(getPort()));
}
