#include "ValueListSpInstance.h"
#include "ValueListSp.h"
#include "db_SP.h"

QString ValueListSpInstance::get() const
{
    return m_currentValue;
}

void ValueListSpInstance::set(const QString &v)
{
    const QStringList& vs = dynamic_cast<const ValueListSp*>(getClass())->getValues();
    if(vs.contains(v))
    {
        m_currentValue = v;
    }
    else
    {
        m_currentValue = vs[dynamic_cast<const ValueListSp*>(getClass())->getDefaultValuesIndex()];
    }
}

