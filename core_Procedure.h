#ifndef CORE_PROCEDURE_H
#define CORE_PROCEDURE_H

#include "core_StoredMethod.h"

class Procedure : public StoredMethod
{
public:
    Procedure();
    Procedure(const QString& pname);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
};

#endif // CORE_PROCEDURE_H
