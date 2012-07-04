#ifndef VALUELISTSPINSTANCE_H
#define VALUELISTSPINSTANCE_H

#include "SpInstance.h"
#include "db_SP.h"

class ValueListSpInstance : public SpInstance
{
public:
    ValueListSpInstance(const Sp* theClass) : SpInstance(theClass), m_currentValue() {}
    ValueListSpInstance(const Sp* theClass, const QString& uid) : SpInstance(theClass)
    {
        setForcedUid(uid);
    }
    virtual QString get() const;
    virtual void set(const QString& v);

private:
    QString m_currentValue;
};

#endif // VALUELISTSPINSTANCE_H
