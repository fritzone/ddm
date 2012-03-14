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
    bool h = m_spInstances.contains(engine);
    if(!h) m_spInstances.insert(engine, t);

    for(int i=0; i<allSps.size(); i++)
    {
        if(allSps.at(i)->getReferredObjectClassUid() == uid)
        {
            m_spInstances[engine].append(allSps.at(i)->instantiate());
        }
    }
}

QVector<SpInstance*>& ObjectWithSpInstances::getSpInstances(const DatabaseEngine* engine)
{
    if(!m_spInstances.contains(engine))
    {
        QVector<SpInstance*> t;
        m_spInstances[engine] = t;
    }

    return m_spInstances[engine];
}

void ObjectWithSpInstances::serialize_spinstances(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement dbElements = doc.createElement("DbEnginesSps");
    QList<const DatabaseEngine*> k = m_spInstances.keys();
    for(int dbI = 0; dbI<k.size(); dbI++)
    {
        QDomElement dbElement = doc.createElement("DbEngineSps");
        dbElement.setAttribute("Name", k.at(dbI)->getDatabaseEngineName());
        for(int i=0; i<m_spInstances[k.at(dbI)].size(); i++)
        {
            m_spInstances[k.at(dbI)].at(i)->serialize(doc, dbElement);
        }
        dbElements.appendChild(dbElement);
    }
    parent.appendChild(dbElements);
}
