#include "MySqlConnection.h"
#include "db_DatabaseEngine.h"
#include "strings.h"
#include "db_DatabaseEngineManager.h"

MySqlConnection::MySqlConnection(const QString& name,
                                 const QString& host, const QString& user, const QString& pass, const QString& db,
                                 bool savePw, bool autoConnect, int port):
    AuthenticatedConnection(name, host, port, db, user, pass, savePw, autoConnect)
{
    m_engine = DatabaseEngineManager::instance().engine(strMySql);
}

void MySqlConnection::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement connectionElement = doc.createElement("Connection");
    connectionElement.setAttribute("Name", getName());
    connectionElement.setAttribute("Host", getHost());
    connectionElement.setAttribute("Pass", getPassword());
    connectionElement.setAttribute("Port", getPort());
    connectionElement.setAttribute("DB",   getDb());
    connectionElement.setAttribute("User", getUser());
    connectionElement.setAttribute("DbType", strMySql);
    connectionElement.setAttribute("AutoConnect", m_autoConnect);
    connectionElement.setAttribute("LastState", m_state);
    parent.appendChild(connectionElement);
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
