#ifndef CORE_PROCEDURE_H
#define CORE_PROCEDURE_H

#include "core_StoredMethod.h"

class Procedure : public StoredMethod
{
public:
    explicit Procedure() {}

    Procedure(const QString& pname, const QString &uid, Version *v, bool guided);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
    virtual QString keyword()
    {
        return "PROCEDURE";
    }
    virtual void updateGui();

    static const QVector<Procedure*>& family(const Procedure&);
};

#endif // CORE_PROCEDURE_H
