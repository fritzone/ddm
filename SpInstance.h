#ifndef SPINSTANCE_H
#define SPINSTANCE_H

#include "SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "db_SP.h"

#include <QString>

class Sp;

/**
 * Represents an instance of an SP object.
 */
class SpInstance : public SerializableElement, public ObjectWithUid
{
public:
    SpInstance(const Sp* theClass) : SerializableElement(), ObjectWithUid(QUuid::createUuid().toString()), m_class(theClass)
    {}

    SpInstance(const Sp* theClass, const QString& uid) : SerializableElement(), ObjectWithUid(uid), m_class(theClass)
    {}

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;
    virtual QString get() const = 0;
    virtual void set(const QString& v) = 0;

    virtual QUuid getClassUid() const
    {
        return m_class->getSqlRoleUid();
    }

    const Sp* getClass() const
    {
        return m_class;
    }

private:

    const Sp* m_class;
};

#endif // SPINSTANCE_H
