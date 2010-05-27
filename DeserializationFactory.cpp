#include "DeserializationFactory.h"
#include "UserDataType.h"
#include "Index.h"
#include "Table.h"
#include "Column.h"
#include "MajorVersion.h"

#include <QStringList>

DeserializationFactory::DeserializationFactory()
{
}

UserDataType* DeserializationFactory::createUserDataType(const QDomDocument& doc, const QDomElement& element)
{
    QString name = element.attribute("Name");
    QString type = element.attribute("Type");
    QString sqlType = element.attribute("SqlType");
    QString size = element.attribute("Size");
    QString defaultValue = element.attribute("DefaultValue");
    QString codepage = element.attribute("Codepage");
    QString isUnsigned = element.attribute("Unsigned");
    QString canBeNull = element.attribute("CanBeNull");
    QString autoInc = element.attribute("CanBeNull");
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

    UserDataType* result = new UserDataType(name, type, sqlType, size, defaultValue, codepage, miscValues, isUnsigned=="1", description, canBeNull == "1", autoInc == "1");

    return result;

}

Index* DeserializationFactory::createIndex(Table* table, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString type = element.attribute("Type");

    Index* result = new Index(name, type);

    for(int i=0; i<element.childNodes().size(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Columns")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().size(); j++) // iterating through the "Column" nodes
            {
                result->addColumn(table->getColumn(element.childNodes().at(i).childNodes().at(j).toElement().attribute("Name")));    // finding the Name attribute
            }
        }
    }
}

MajorVersion* DeserializationFactory::createMajorVersion(const QDomDocument &doc, const QDomElement &element)
{
    QString name = "";

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Version")
        {
            name = element.childNodes().at(i).firstChild().nodeValue();// first child is a text node
        }
    }

    MajorVersion* result = new MajorVersion(name);

    // getting the data types
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "DataTypes")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                UserDataType* dt = createUserDataType(doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addNewDataType(dt);
            }
        }
    }
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Tables")   // in a well formatted result, the Tables child node is always after the datatypes
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Table* tab = createTable(result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addTable(tab);
            }
        }
    }
}

Column* DeserializationFactory::createColumn(MajorVersion* ver, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString pk = element.attribute("PK");
    QString type = element.attribute("Type");
    UserDataType* udt = ver->getDataType(type);

    Column* col = new Column(name, udt, pk == "1");
    return col;
}

Table* DeserializationFactory::createTable(MajorVersion* ver, const QDomDocument &doc, const QDomElement &element)
{
    Table* result = new Table();
    QString name = element.attribute("Name");
    result->setName(name);

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Description")
        {
            result->setDescription(element.childNodes().at(i).firstChild().nodeValue());
        }
        if(element.childNodes().at(i).nodeName() == "Columns")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Column* col = createColumn(ver, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addColumn(col);
            }
        }
    }
}
