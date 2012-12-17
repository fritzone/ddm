#include "dgram_DiagramNoteDescriptor.h"
#include "uids.h"

void DiagramNoteDescriptor::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement tableElement = doc.createElement("Note");      // will hold the data in this element
    tableElement.setAttribute("x", getX());
    tableElement.setAttribute("y", getY());
    tableElement.setAttribute("frame", m_framed);

    QDomElement textElement = doc.createElement("Text");
    QDomCDATASection cdata = doc.createCDATASection(getText());
    textElement.appendChild(cdata);
    tableElement.appendChild(textElement);

    parent.appendChild(tableElement);
}

QUuid DiagramNoteDescriptor::getClassUid() const
{
    return QUuid(uidDiagramNoteDescriptor);
}

CloneableElement* DiagramNoteDescriptor::clone(Version* /*sourceVersion*/, Version* targetVersion)
{
    DiagramNoteDescriptor* result = new DiagramNoteDescriptor(getText(), getX(), getY(), m_framed, targetVersion);
    result->setSourceUid(getObjectUid());
    return result;
}
