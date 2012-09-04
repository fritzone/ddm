#ifndef DIAGRAMTABLEDESCRIPTOR_H
#define DIAGRAMTABLEDESCRIPTOR_H

#include "dgram_DiagramObjectDescriptor.h"

class DiagramTableDescriptor : public DiagramObjectDescriptor
{
public:

    /**
     * Constructor
     */
    DiagramTableDescriptor(const QString& tableName, int x, int y) : DiagramObjectDescriptor(tableName, x, y) {}

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
};

#endif // DIAGRAMTABLEDESCRIPTOR_H
