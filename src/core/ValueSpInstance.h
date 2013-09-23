#ifndef VALUESPINSTANCE_H
#define VALUESPINSTANCE_H

#include "SpInstance.h"
#include "db_SP.h"

class ValueSpInstance : public SpInstance
{
public:
    ValueSpInstance(const Sp* theClass, Version* v) : SpInstance(theClass, v), m_currentValue() {}
    ValueSpInstance(const Sp* theClass, const QString& uid, Version* v) : SpInstance(theClass, v)
    {
        setForcedUid(uid);
    }
    virtual QString get() const;
    virtual void set(const QString& v);

private:
    QString m_currentValue;
};

#endif // VALUESPINSTANCE_H

