#include "core_Entity.h"
#include "uids.h"
#include "UidWarehouse.h"
#include "core_Repository.h"
#include "core_Role.h"

Entity::Entity(const QString &name, const QUuid& classUid, const QUuid &targetClassUid, bool hasDesc, bool supportSps) :
    NamedItem(name),
    ObjectWithUid(QUuid::createUuid()),
    m_classUid(targetClassUid), m_targetClassUid(targetClassUid),
    m_hasDescription(hasDesc), m_supportsSps(supportSps)
{
    qDebug() << "CREA:" << classUid;
    UidWarehouse::instance().addElement(targetClassUid, this);
}

QUuid Entity::getClassUid() const
{
    return QUuid(m_classUid);
}

void Entity::setAttribute(const QString& a, const QString& v)
{
    for(int i=0; i<m_attr.size(); i++)
    {
        if(m_attr[i]->name == a)
        {
            const QVector<Role*> roles = Repository::instance()->getRoles();
            for(int j=0; j<roles.size(); j++)
            {
                if(roles[j]->getName() == v)
                {
                    m_attr[i]->roleUid = roles[j]->getUid();
                }
            }
        }
    }
}

Entity::Attribute* Entity::getAttribute(const QString &attr)
{
    for(int i=0; i<m_attr.size(); i++)
    {
        if(m_attr[i]->name == attr)
        {
            return m_attr[i];
        }
    }
}
