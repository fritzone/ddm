#include "dgram_DiagramTableDescriptor.h"
#include "uids.h"

void DiagramTableDescriptor::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableElement = doc.createElement("Table");      // will hold the data in this element
    tableElement.setAttribute("Name", getText());
    tableElement.setAttribute("x", getX());
    tableElement.setAttribute("y", getY());

    parent.appendChild(tableElement);
}


QUuid DiagramTableDescriptor::getClassUid() const
{
    return QUuid(uidDiagramTableDescriptor);
}

CloneableElement* DiagramTableDescriptor::clone(Version* sourceVersion, Version* targetVersion)
{
    DiagramTableDescriptor* dtd = new DiagramTableDescriptor(getText(), getX(), getY(), targetVersion);
    dtd->setSourceUid(getObjectUid());
    return dtd;
}
