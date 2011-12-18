#include "core_ConnectionManager.h"
#include <QDomDocument>
#include <QFile>

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
    m_connections.clear();
    QDomDocument doc ("DDM-CONNECTIONS");
    QFile file ("ddm_connections.cfx");
    if (!file.open(QIODevice::ReadOnly)) return;
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();
    QDomElement docElem = doc.documentElement();
    for(int i=0; i<docElem.childNodes().size(); i++)
    {
        if(docElem.childNodes().at(i).nodeName() == "Connection")
        {
            QString name = docElem.childNodes().at(i).toElement().attribute("Name");
            QString host = docElem.childNodes().at(i).toElement().attribute("Host");
            QString pass = docElem.childNodes().at(i).toElement().attribute("Pass");
            QString user = docElem.childNodes().at(i).toElement().attribute("User");
            QString db = docElem.childNodes().at(i).toElement().attribute("DB");
            QString dbType = docElem.childNodes().at(i).toElement().attribute("DbType");
            QString ac = docElem.childNodes().at(i).toElement().attribute("AutoConnect");

            Connection* c = new Connection(name, host, user, pass, db, true, ac=="1");
            m_connections.append(c);

            if(ac=="1")
            {
                c->tryConnect();
            }
        }
    }
}

void ConnectionManager::saveConnections()
{
    QDomDocument doc("DDM-CONNECTIONS");
    QDomElement root = doc.createElement("Connections");

    for(int i=0; i<m_connections.size(); i++)
    {
        m_connections.at(i)->serialize(doc, root);
    }

    doc.appendChild(root);

    QString xml = doc.toString();
    QFile f1("ddm_connections.cfx");
    if (!f1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    f1.write(xml.toAscii());
    f1.close();
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
