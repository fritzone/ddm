#include "core_Entity.h"
#include "uids.h"
#include "UidWarehouse.h"

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
