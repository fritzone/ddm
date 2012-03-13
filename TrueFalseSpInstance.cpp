#include "TrueFalseSpInstance.h"

void TrueFalseSpInstance::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement spElement = doc.createElement("SpInstance");
    getClass()->serialize(doc, spElement);
    spElement.setAttribute("Value", get());

    parent.appendChild(spElement);
}
