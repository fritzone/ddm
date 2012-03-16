#ifndef TRUEFALSESPINSTANCE_H
#define TRUEFALSESPINSTANCE_H

#include "SpInstance.h"
#include "db_SP.h"

class TrueFalseSpInstance : public SpInstance
{
public:
    TrueFalseSpInstance(const Sp* theClass) : SpInstance(theClass), m_value(false) {}
    TrueFalseSpInstance(const Sp* theClass, const QString& uid) : SpInstance(theClass)
    {
        setForcedUid(uid);
    }

    virtual QString get() const;
    virtual void set(const QString& v);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

private:
    bool m_value;
};

#endif // TRUEFALSESPINSTANCE_H
