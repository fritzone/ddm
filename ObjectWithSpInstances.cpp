#include "ObjectWithSpInstances.h"
#include "db_SP.h"
#include "Workspace.h"
#include "Version.h"
#include "db_DatabaseEngine.h"
#include "SpInstance.h"

ObjectWithSpInstances::ObjectWithSpInstances() : m_spInstances()
{
}

void ObjectWithSpInstances::initializeFor(const DatabaseEngine* engine, const QUuid &uid)
{
    const QVector<Sp*> allSps = engine->getDatabaseSpecificProperties();
    QVector<SpInstance*> t;
    bool h = m_spInstances.contains(engine->getDatabaseEngineName());
    if(!h) m_spInstances.insert(engine->getDatabaseEngineName(), t);

    for(int i=0; i<allSps.size(); i++)
    {
        if(allSps.at(i)->getReferredObjectClassUid() == uid)
        {
            SpInstance* spi = allSps.at(i)->instantiate();
            m_spInstances[engine->getDatabaseEngineName()].append(spi);
        }
    }
}

void ObjectWithSpInstances::initializeRemainingSps(const DatabaseEngine* engine, const QUuid& uid)
{
    const QVector<Sp*> allSps = engine->getDatabaseSpecificProperties();
    QVector<SpInstance*> t;
    bool h = m_spInstances.contains(engine->getDatabaseEngineName());
    if(!h) m_spInstances.insert(engine->getDatabaseEngineName(), t);

    for(int i=0; i<allSps.size(); i++)
    {
        if(getInstanceForSqlRoleUid(engine, allSps.at(i)->getSqlRoleUid()) == 0)
        {
            if(allSps.at(i)->getReferredObjectClassUid() == uid)
            {
                SpInstance* spi = allSps.at(i)->instantiate();
                m_spInstances[engine->getDatabaseEngineName()].append(spi);
            }
        }
    }
}

QVector<SpInstance*>& ObjectWithSpInstances::getSpInstances(const DatabaseEngine* engine)
{
    if(!m_spInstances.contains(engine->getDatabaseEngineName()))
    {
        QVector<SpInstance*> t;
        m_spInstances[engine->getDatabaseEngineName()] = t;
    }

    return m_spInstances[engine->getDatabaseEngineName()];
}

void ObjectWithSpInstances::serialize_spinstances(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement dbElements = doc.createElement("DbEnginesSps");
    QList<QString> k = m_spInstances.keys();
    for(int dbI = 0; dbI<k.size(); dbI++)
    {
        QDomElement dbElement = doc.createElement("DbEngineSps");
        dbElement.setAttribute("Name", k.at(dbI));
        for(int i=0; i<m_spInstances[k.at(dbI)].size(); i++)
        {
            m_spInstances[k.at(dbI)].at(i)->serialize(doc, dbElement);
        }
        dbElements.appendChild(dbElement);
    }
    parent.appendChild(dbElements);
}

SpInstance* ObjectWithSpInstances::getInstanceForSqlRoleUid(const DatabaseEngine *eng, const QString &sqlRoleUid)
{
    qDebug() << "getInstance="<< eng<< " as " << eng->getDatabaseEngineName();
    if(!m_spInstances.contains(eng->getDatabaseEngineName()))
    {
        qDebug() << "NO DB";
        return 0;
    }

    QVector<SpInstance*>& v = m_spInstances[eng->getDatabaseEngineName()];
    for(int i=0; i<v.size(); i++)
    {
        qDebug() << "VVV=" << v.at(i)->getObjectUid() << " +++ " << sqlRoleUid;
        if(v.at(i)->getClass()->getSqlRoleUid() == sqlRoleUid)
        {
            return v.at(i);
        }
    }
    qDebug() << "No object";
    return 0;
}

SpInstance* ObjectWithSpInstances::getInstance(const DatabaseEngine *eng, const QString &objectUid)
{
    qDebug() << "getInstance="<< eng<< " as " << eng->getDatabaseEngineName();
    if(!m_spInstances.contains(eng->getDatabaseEngineName()))
    {
        qDebug() << "NO DB";
        return 0;
    }

    QVector<SpInstance*>& v = m_spInstances[eng->getDatabaseEngineName()];
    for(int i=0; i<v.size(); i++)
    {
        qDebug() << "VVV=" << v.at(i)->getObjectUid() << " +++ " << objectUid;
        if(v.at(i)->getObjectUid() == objectUid)
        {
            return v.at(i);
        }
    }
    qDebug() << "No object";
    return 0;
}

void ObjectWithSpInstances::addSpInstance(const DatabaseEngine *eng, SpInstance *spi)
{
    qDebug() << "addSpI="<< eng<< " as " << eng->getDatabaseEngineName();
    if(!m_spInstances.contains(eng->getDatabaseEngineName()))
    {
        QVector<SpInstance*> t;
        m_spInstances[eng->getDatabaseEngineName()] = t;
    }

    m_spInstances[eng->getDatabaseEngineName()].append(spi);
}
