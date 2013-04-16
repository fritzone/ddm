#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include <Connection.h>

class MySqlConnection : public Connection
{
public:


    MySqlConnection(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, bool savePw, bool autoConnect, int port);

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual void saveIntoSettings(QSettings& s);
    virtual QString getDbType() const
    {
        return "MySql";
    }

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

    void resetTo(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, int port, bool savePw, bool autoConnect);

private:

    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_db;
    int m_port;
    bool m_savePw;
};

#endif // MYSQLCONNECTION_H
