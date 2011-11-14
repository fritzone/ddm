#include "core_Connection.h"

Connection::Connection(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, bool savePw, bool autoConnect):
        NamedItem(name), SerializableElement(), TreeItem(),
        m_host(host), m_user(user), m_pass(pass), m_db(db), m_dbType("MySQL"), m_savePw(savePw), m_autoConnect(autoConnect)
{
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
    parent.appendChild(connectionElement);
}
