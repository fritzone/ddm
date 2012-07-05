#ifndef SPINSTANCE_H
#define SPINSTANCE_H

#include "SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "db_SP.h"
#include "core_CloneableElement.h"

#include <QString>

class Sp;
class Version;

/**
 * Represents an instance of an SP object.
 */
class SpInstance : public SerializableElement, public ObjectWithUid, public CloneableElement
{
public:
    SpInstance(const Sp* theClass);

    SpInstance(const Sp* theClass, const QString& uid);

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QString get() const = 0;
    virtual void set(const QString& v) = 0;

    virtual QUuid getClassUid() const;

    const Sp* getClass() const;

    CloneableElement* clone(Version * /*sourceVersion*/, Version * /*targetVersion*/);

private:

    const Sp* m_class;
};

#endif // SPINSTANCE_H
