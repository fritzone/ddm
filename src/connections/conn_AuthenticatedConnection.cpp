#include "conn_AuthenticatedConnection.h"

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
