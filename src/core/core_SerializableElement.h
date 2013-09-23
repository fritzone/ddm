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

    /**
     * Serializes this element into the given document, as a child of parent
     * @param doc - the XML document
     * @param parent - the parent node
     */
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;

    /**
     * Destructor
     */
    virtual ~SerializableElement() {}
};

#endif // _SERIALIZABLEELEMENT_H_

