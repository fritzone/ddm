#ifndef CORE_FUNCTION_H
#define CORE_FUNCTION_H

#include "core_StoredMethod.h"

class Function : public StoredMethod
{
public:
    Function();
    Function(const QString& pname);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
};

#endif // CORE_FUNCTION_H
