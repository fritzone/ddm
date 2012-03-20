#include "TrueFalseSpInstance.h"

QString TrueFalseSpInstance::get() const
{
    if(m_value) return "TRUE";
    return "FALSE";
}

void TrueFalseSpInstance::set(const QString &v)
{
    qDebug() << "SET:" << v <<" " << getObjectUid();
    if(v.toUpper() == "TRUE") m_value = true;
    else
    if(v.toUpper() == "FALSE") m_value = false;
}
