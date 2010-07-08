#ifndef DIAGRAMNOTEDESCRIPTOR_H
#define DIAGRAMNOTEDESCRIPTOR_H

#include "DiagramObjectDescriptor.h"

class DiagramNoteDescriptor : public DiagramObjectDescriptor
{
public:

    DiagramNoteDescriptor(const QString& txt, int x, int y, bool framed) : DiagramObjectDescriptor(txt, x, y), m_framed(framed) {}

    bool isFramed() {return m_framed;}

    void setFramed(bool f) {m_framed = f;}

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const
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
private:

    bool m_framed;
};

#endif // DIAGRAMNOTEDESCRIPTOR_H
