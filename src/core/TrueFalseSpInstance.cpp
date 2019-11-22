#include "TrueFalseSpInstance.h"
#include "ddm_strings.h"

QString TrueFalseSpInstance::get() const
{
    if(m_value)
    {
        return strTrue;
    }
    else
    {
        return strFalse;
    }
}

void TrueFalseSpInstance::set(const QString &v)
{
    if(v.toUpper() == strTrue)
    {
        m_value = true;
    }
    else
    if(v.toUpper() == strFalse)
    {
        m_value = false;
    }
}
