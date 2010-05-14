#include "DeserializationFactory.h"
#include "UserDataType.h"

#include <QStringList>

DeserializationFactory::DeserializationFactory()
{
}

UserDataType* DeserializationFactory::createUserDataType(QDomDocument& doc, QDomElement& element)
{
    QString name = element.attribute("Name");
    QString type = element.attribute("Type");
    QString sqlType = element.attribute("SqlType");
    QString size = element.attribute("Size");
    QString defaultValue = element.attribute("DefaultValue");
    QString codepage = element.attribute("Codepage");
    QString isUnsigned = element.attribute("Unsigned");
    QString canBeNull = element.attribute("CanBeNull");
    QString description = "";
    QStringList miscValues;

    for(int i=0; i<element.childNodes().size(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Description")
        {
            QDomNode descNode = element.childNodes().at(i);
            description = descNode.firstChild().nodeValue();
        }

        if(element.childNodes().at(i).nodeName() == "Values")
        {
            QDomNode valuesNode = element.childNodes().at(i);
            for(int j=0; j<valuesNode.childNodes().count(); j++)
            {
                miscValues.append(valuesNode.childNodes().at(j).firstChild().nodeValue());
            }
        }
    }

    UserDataType* result = new UserDataType(name, type, sqlType, size, defaultValue, codepage, miscValues, isUnsigned=="1", description, canBeNull == "1");

    return result;

}
