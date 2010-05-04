#ifndef _SERIALIZABLEELEMENT_H_
#define _SERIALIZABLEELEMENT_H_


/**
 * All the classes that can be serialized should be derived from this class
 */
class SerializableElement
{
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;
};

#endif // _SERIALIZABLEELEMENT_H_

