#ifndef DESERIALIZATIONFACTORY_H
#define DESERIALIZATIONFACTORY_H

#include <QDomDocument>
#include <QDomElement>

class UserDataType;

class DeserializationFactory
{
public:

    static UserDataType* createUserDataType(QDomDocument& doc, QDomElement& element);

private:
    DeserializationFactory();
};

#endif // DESERIALIZATIONFACTORY_H
