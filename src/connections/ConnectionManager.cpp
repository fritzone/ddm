#include "ConnectionManager.h"
#include "strings.h"
#include "MySqlConnection.h"

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

        QString dbt = s.value(strDbType).toString();

        Connection*c = createConnection(dbt, s);

        s.endGroup();

        m_connections.append(c);

        if(c->getAC())
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

        // common data
        s.setValue(strDbType, m_connections.at(i)->getDbType());
        s.setValue(strAutoConnect, m_connections.at(i)->getAC());

        // and save the sepcific data
        m_connections[i]->saveIntoSettings(s);

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

Connection* ConnectionManager::createConnection(const QString &dbType, const QSettings &s)
{
    bool ac = s.value(strAutoConnect).toBool();

    if(dbType == "MYSQL")
    {
        QString name = s.value(strName).toString();
        QString host = s.value(strHost).toString();
        QString pass = s.value(strPass).toString();
        QString user = s.value(strUser).toString();
        int port = 3306;
        if(s.contains(strPort))
        {
            QVariant portV = s.value(strPort);
            port = portV.toString().toInt();
        }
        QString db = s.value(strDB).toString();

        int lastState = s.value("LastState").toInt();
        Connection* c = new MySqlConnection(name, host, user, pass, db, true, ac, port);
        c->setState((ConnectionState)(lastState));
    }
}
