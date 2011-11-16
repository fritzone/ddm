#include "core_Connection.h"
#include "db_DatabaseEngine.h"

#include <QMessageBox>
// TODO: remove all QT reference from the core files, instead create an Error Manager class which will show all the error messages

Connection::Connection(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, bool savePw, bool autoConnect):
        NamedItem(name), SerializableElement(), TreeItem(),
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
    parent.appendChild(connectionElement);
}

bool Connection::tryConnect()
{
    if(!m_engine->tryConnect(m_host, m_user, m_pass, m_db))
    {
        QMessageBox::critical(0, QObject::tr("Error"), m_engine->getLastError(), QMessageBox::Ok)        ;
        m_state = FAILED;
        return false;
    }
    m_state = CONNECTED;
    return true;
}
