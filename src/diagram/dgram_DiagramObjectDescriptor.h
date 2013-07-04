#ifndef DIAGRAMOBJECTDESCRIPTOR_H
#define DIAGRAMOBJECTDESCRIPTOR_H

#include "core_SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "core_CloneableElement.h"
#include "core_VersionElement.h"

#include <QString>

class DiagramObjectDescriptor : public SerializableElement, public VersionElement, public CloneableElement
{
public:

    DiagramObjectDescriptor(const QString& txt, int x, int y, Version *v) : VersionElement(QUuid::createUuid().toString(), v), m_txt(txt), m_x(x), m_y(y) {}


    QString getText() const
    {
        return m_txt;
    }

    int getX() const
    {
        return m_x;
    }

    int getY() const
    {
        return m_y;
    }

    void setX(int x)
    {
        m_x = x;
    }

    void setY(int y)
    {
        m_y = y;
    }

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;
    virtual QUuid getClassUid() const = 0;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion) = 0;

private:

    // this is: for objects isntantiated from this class the table name.
    // for obejcts derived from the Note descriptor the tect of the note.
    // for objects derived from the FK descriptor the name of the first table
    QString m_txt;
    int m_x;
    int m_y;
};

#endif // DIAGRAMOBJECTDESCRIPTOR_H
