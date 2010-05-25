#ifndef _SERIALIZABLEELEMENT_H_
#define _SERIALIZABLEELEMENT_H_

#include <QDomDocument>
#include <QDomElement>

/**
 * All the classes that can be serialized should be derived from this class
 */
class SerializableElement
{
public:
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;

    virtual ~SerializableElement() {}
};

#endif // _SERIALIZABLEELEMENT_H_

