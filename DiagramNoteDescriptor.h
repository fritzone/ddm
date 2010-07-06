#ifndef DIAGRAMNOTEDESCRIPTOR_H
#define DIAGRAMNOTEDESCRIPTOR_H

#include "DiagramObjectDescriptor.h"

#include <QString>

class DiagramNoteDescriptor : public DiagramObjectDescriptor
{
public:

    DiagramNoteDescriptor(const QString& txt, int x, int y, bool framed) : DiagramObjectDescriptor(txt, x, y), m_framed(framed) {}


    bool isFramed() {return m_framed;}

private:

    bool m_framed;
};

#endif // DIAGRAMNOTEDESCRIPTOR_H
