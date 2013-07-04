#ifndef CONN_AUTHENTICATEDCONNECTION_H
#define CONN_AUTHENTICATEDCONNECTION_H

#include "Connection.h"

#include <QString>

class AuthenticatedConnection : public Connection
{
public:

    AuthenticatedConnection(const QString& name,
                            const QString& host,
                            int port,
                            const QString& db,
                            const QString& user,
                            const QString& pass,
                            bool savePw,
                            bool autoConnect);

    virtual QString getFullLocation() const
    {
        return m_db+"@"+m_host;
    }

    QString getHost() const
    {
        return m_host;
    }
    QString getDb() const
    {
        return m_db;
    }

    QString getUser() const
    {
        return m_user;
    }
    QString getPassword() const
    {
        return m_pass;
    }

    int getPort() const
    {
        return m_port;
    }

    void resetTo(const QString& name,
                 const QString& host,
                 const QString& user,
                 const QString& pass,
                 const QString& db,
                 int port,
                 bool savePw,
                 bool autoConnect);

private:

    QString m_host;
    int m_port;
    QString m_db;
    QString m_user;
    QString m_pass;
    bool m_savePw;

};


#endif // CONN_AUTHENTICATEDCONNECTION_H
