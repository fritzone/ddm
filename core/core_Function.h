#ifndef CORE_FUNCTION_H
#define CORE_FUNCTION_H

#include "core_StoredMethod.h"
#include "core_ObjectWithUid.h"

class Function : public StoredMethod
{
public:
    Function(const QString& pname, const QString& guid);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
    virtual void updateGui();

    virtual QString keyword()
    {
        return "FUNCTION";
    }
};

#endif // CORE_FUNCTION_H
