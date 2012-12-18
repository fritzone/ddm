#ifndef TRUEFALSESPINSTANCE_H
#define TRUEFALSESPINSTANCE_H

#include "SpInstance.h"
#include "db_SP.h"

class TrueFalseSpInstance : public SpInstance
{
public:
    TrueFalseSpInstance(const Sp* theClass, Version* v) : SpInstance(theClass, v), m_value(false) {}
    TrueFalseSpInstance(const Sp* theClass, const QString& uid, Version* v) : SpInstance(theClass, v)
    {
        setForcedUid(uid);
    }

    virtual QString get() const;
    virtual void set(const QString& v);

private:
    bool m_value;
};

#endif // TRUEFALSESPINSTANCE_H
