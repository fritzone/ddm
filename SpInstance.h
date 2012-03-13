#ifndef SPINSTANCE_H
#define SPINSTANCE_H

#include "SerializableElement.h"

#include <QString>

class Sp;

/**
 * Represents an instance of an SP object.
 */
class SpInstance : public SerializableElement
{
public:
    SpInstance(const Sp* theClass) : m_class(theClass)
    {}

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;
    virtual QString get() const = 0;

    const Sp* getClass() const
    {
        return m_class;
    }

private:

    const Sp* m_class;
};

#endif // SPINSTANCE_H
