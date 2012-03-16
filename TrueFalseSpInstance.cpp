#include "TrueFalseSpInstance.h"

void TrueFalseSpInstance::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement spElement = doc.createElement("SpInstance");
    getClass()->serialize(doc, spElement);
    spElement.setAttribute("Value", get());
    spElement.setAttribute("uid", getObjectUid());
    spElement.setAttribute("class-uid", getClassUid());

    parent.appendChild(spElement);
}

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
