#ifndef CORE_CONNECTIONMANAGER_H
#define CORE_CONNECTIONMANAGER_H

#include "core_Connection.h"
#include <QVector>

class ConnectionManager
{
public:
    static ConnectionManager* instance();
public:

    void loadConnections();
    void saveConnections();
    const QVector<Connection*>& connections() const
    {
        return m_connections;
    }
    void addConnection(Connection* c)
    {
        m_connections.append(c);
    }
    void shutDown()
    {
        saveConnections();
    }

    Connection* getConnection(const QString& name) const;
    bool deleteConnection(const QString& name);

private:
    ConnectionManager();
    ~ConnectionManager();

private:
    static ConnectionManager* m_instance;

private:
    QVector<Connection*> m_connections;

};

#endif // CORE_CONNECTIONMANAGER_H
