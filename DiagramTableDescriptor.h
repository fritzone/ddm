#ifndef DIAGRAMTABLEDESCRIPTOR_H
#define DIAGRAMTABLEDESCRIPTOR_H

#include "DiagramObjectDescriptor.h"

class DiagramTableDescriptor : public DiagramObjectDescriptor
{
public:

    DiagramTableDescriptor(const QString& tableName, int x, int y) : DiagramObjectDescriptor(tableName, x, y) {}

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
};

#endif // DIAGRAMTABLEDESCRIPTOR_H
