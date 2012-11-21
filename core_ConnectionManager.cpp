#include "core_ConnectionManager.h"
#include "strings.h"

#include <QSettings>

ConnectionManager* ConnectionManager::m_instance = 0;

ConnectionManager* ConnectionManager::instance()
{
    if(m_instance == 0)
    {
        m_instance = new ConnectionManager();
    }

    return m_instance;
}

ConnectionManager::~ConnectionManager()
{
    saveConnections();
}

ConnectionManager::ConnectionManager()
{
    loadConnections();
}

void ConnectionManager::loadConnections()
{
    QSettings s(strUnauthorizedFrog, strDDM);
    int l = s.value(strConnectionsCount, QVariant(-1)).toInt();
    if(l == -1) return;

    for(int i=0; i<l; i++)
    {
        QString connI = QString("Connection");
        connI.append(QString::number(i));
        s.beginGroup(connI);
        QString name = s.value(strName).toString();
        QString host = s.value(strHost).toString();
        QString pass = s.value(strPass).toString();
        QString user = s.value(strUser).toString();
        int port = s.value(strPort).toString().toInt();
        QString db = s.value(strDB).toString();
        bool ac = s.value(strAutoConnect).toBool();
        QString dbt = s.value(strDbType).toString();
        int lastState = s.value("LastState").toInt();
        s.endGroup();

        Connection* c = new Connection(name, host, user, pass, db, true, ac, port);
        c->setState((ConnectionState)(lastState));
        m_connections.append(c);

        if(ac)
        {
            c->tryConnect();
        }
    }
}

void ConnectionManager::saveConnections()
{
    QSettings s(strUnauthorizedFrog, strDDM);
    s.setValue(strConnectionsCount, m_connections.size());

    for(int i=0; i<m_connections.size(); i++)
    {
        QString connI = QString("Connection");
        connI.append(QString::number(i));
        s.beginGroup(connI);
        s.setValue(strName, m_connections.at(i)->getName());
        s.setValue(strHost, m_connections.at(i)->getHost());
        s.setValue(strPass, m_connections.at(i)->getPassword());
        s.setValue(strUser, m_connections.at(i)->getUser());
        s.setValue(strDB, m_connections.at(i)->getDb());
        s.setValue(strPort, QString::number(m_connections.at(i)->getPort()));
        s.setValue(strAutoConnect, m_connections.at(i)->getAC());
        s.setValue(strDbType, m_connections.at(i)->getDbType());
        s.endGroup();
    }

    s.sync();
}

Connection* ConnectionManager::getConnection(const QString& name) const
{
    for(int i=0; i<m_connections.size(); i++)
    {
        if(m_connections.at(i)->getName() == name)
        {
            return m_connections.at(i);
        }
    }
    return 0;
}

bool ConnectionManager::deleteConnection(const QString& name)
{
    Connection* c = getConnection(name);
    if(c)
    {
        m_connections.remove(m_connections.indexOf(c));
        return true;
    }
    return false;
}
