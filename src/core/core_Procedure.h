#ifndef CORE_PROCEDURE_H
#define CORE_PROCEDURE_H

#include "core_StoredMethod.h"

class Procedure : public StoredMethod
{
public:
    Procedure(const QString& pname, const QString &uid, Version *v);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
    virtual QString keyword()
    {
        return "PROCEDURE";
    }
    virtual void updateGui();
};

#endif // CORE_PROCEDURE_H
