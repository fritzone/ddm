#ifndef DIAGRAMNOTEDESCRIPTOR_H
#define DIAGRAMNOTEDESCRIPTOR_H

#include "dgram_DiagramObjectDescriptor.h"

class DiagramNoteDescriptor : public DiagramObjectDescriptor
{
public:

    DiagramNoteDescriptor(const QString& txt, int x, int y, bool framed, Version *v) : DiagramObjectDescriptor(txt, x, y, v), m_framed(framed) {}

    bool isFramed() {return m_framed;}

    void setFramed(bool f) {m_framed = f;}

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);

private:

    bool m_framed;
};

#endif // DIAGRAMNOTEDESCRIPTOR_H
