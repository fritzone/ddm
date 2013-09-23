#include "ValueSpInstance.h"
#include "ValueSp.h"
#include "db_SP.h"

QString ValueSpInstance::get() const
{
    return m_currentValue;
}

void ValueSpInstance::set(const QString &v)
{
    m_currentValue = v;
}

