#ifndef CORE_PROCEDURE_H
#define CORE_PROCEDURE_H

#include "core_StoredMethod.h"
#include "core_ObjectWithUid.h"

class Procedure : virtual public StoredMethod, virtual public ObjectWithUid
{
public:
    Procedure(const QString& pname, const QString& uid);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
};

#endif // CORE_PROCEDURE_H

// NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (itemGetter)&Version::getProcedure, QString("proc"))
