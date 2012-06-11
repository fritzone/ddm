#include "dgram_DiagramTableDescriptor.h"

void DiagramTableDescriptor::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableElement = doc.createElement("Table");      // will hold the data in this element
    tableElement.setAttribute("Name", getText());
    tableElement.setAttribute("x", getX());
    tableElement.setAttribute("y", getY());

    parent.appendChild(tableElement);

}
