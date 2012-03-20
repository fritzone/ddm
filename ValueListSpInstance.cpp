#include "ValueListSpInstance.h"

QString ValueListSpInstance::get() const
{
    return m_currentValue;
}

void ValueListSpInstance::set(const QString &v)
{
    m_currentValue = v;
}

