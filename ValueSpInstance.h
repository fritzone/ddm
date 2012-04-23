#ifndef VALUESPINSTANCE_H
#define VALUESPINSTANCE_H

#include "SpInstance.h"
#include "db_SP.h"

class ValueSpInstance : public SpInstance
{
public:
    ValueSpInstance(const Sp* theClass) : SpInstance(theClass), m_currentValue() {}
    ValueSpInstance(const Sp* theClass, const QString& uid) : SpInstance(theClass)
    {
        setForcedUid(uid);
    }
    virtual QString get() const;
    virtual void set(const QString& v);

private:
    QString m_currentValue;
};

#endif // VALUESPINSTANCE_H

