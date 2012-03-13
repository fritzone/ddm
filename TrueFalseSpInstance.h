#ifndef TRUEFALSESPINSTANCE_H
#define TRUEFALSESPINSTANCE_H

#include "SpInstance.h"
#include "db_SP.h"

class TrueFalseSpInstance : public SpInstance
{
public:
    TrueFalseSpInstance(const Sp* theClass) : SpInstance(theClass){}

    virtual QString get() const
    {
        return "TRUE";
    }
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
};

#endif // TRUEFALSESPINSTANCE_H
