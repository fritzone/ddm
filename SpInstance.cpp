#include "SpInstance.h"

SpInstance::SpInstance(const Sp* theClass) : SerializableElement(),
    ObjectWithUid(QUuid::createUuid().toString()), m_class(theClass)
{}

SpInstance::SpInstance(const Sp* theClass, const QString& uid) :
    SerializableElement(), ObjectWithUid(uid), m_class(theClass)
{}

void SpInstance::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement spElement = doc.createElement("SpInstance");
    getClass()->serialize(doc, spElement);
    spElement.setAttribute("Value", get());
    spElement.setAttribute("uid", getObjectUid());
    spElement.setAttribute("class-uid", getClassUid());

    parent.appendChild(spElement);
}


CloneableElement* SpInstance::clone(Version */*sourceVersion*/, Version */*targetVersion*/)
{
    SpInstance* result = const_cast<Sp*>(getClass())->instantiate();
    result->setSourceUid(getObjectUid());
    result->setForcedUid(QUuid::createUuid().toString());
    result->set(get());
    return result;
}


QUuid SpInstance::getClassUid() const
{
    return m_class->getSqlRoleUid();
}

const Sp* SpInstance::getClass() const
{
    return m_class;
}
