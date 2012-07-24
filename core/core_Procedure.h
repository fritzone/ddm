#ifndef CORE_PROCEDURE_H
#define CORE_PROCEDURE_H

#include "core_StoredMethod.h"
#include "core_ObjectWithUid.h"

class Procedure : public StoredMethod, public ObjectWithUid
{
public:
    Procedure(const QString& pname, const QString& uid);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
    virtual QString keyword()
    {
        return "PROCEDURE";
    }
};

#endif // CORE_PROCEDURE_H
