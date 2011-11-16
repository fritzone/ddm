#ifndef CORE_CONNECTION_H
#define CORE_CONNECTION_H

#include "SerializableElement.h"
#include "NamedItem.h"
#include "TreeItem.h"

#include <QString>

class DatabaseEngine;

class Connection : virtual public SerializableElement, virtual public NamedItem, virtual public TreeItem
{
public:

    enum ConnectionState
    {
        CONNECTED=1,
        FAILED=2,
        DID_NOT_TRY=3
    };

    Connection(const QString& name, const QString& host, const QString& user, const QString& pass, const QString& db, bool savePw, bool autoConnect);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    QString getHost() const
    {
        return m_host;
    }
    QString getDb() const
    {
        return m_db;
    }

    bool tryConnect();
    ConnectionState getState() const
    {
        return m_state;
    }
    QString getUser() const
    {
        return m_user;
    }
    QString getPassword() const
    {
        return m_pass;
    }

private:

    QString m_host;
    QString m_user;
    QString m_pass;
    QString m_db;
    QString m_dbType;
    bool m_savePw;
    bool m_autoConnect;
    DatabaseEngine* m_engine;
    ConnectionState m_state;
};

#endif // CORE_CONNECTION_H
