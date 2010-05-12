#include "UserDataType.h"

UserDataType::UserDataType(const QString& name, const QString& typeString,
                           const QString& _sqlType, const QString& _s,
                           const QString& _defaultValue, const QString& _cp,
                           const QStringList& _mvs, bool unsi) :
        DataType(name, DataType::getDT_TYPE(typeString)),
        sqlType(_sqlType),
        icon(& DataType::getIcon(DataType::getDT_TYPE(typeString))),
        size(_s), defaultValue(_defaultValue), miscStuff(_mvs), codePage(_cp),
        unsignedDT(unsi)
{
}

UserDataType& UserDataType::operator = (const UserDataType& other)
{
    if(this != &other)
    {
        name = other.name;
        type = other.type;
        sqlType = other.sqlType;
        size = other.size;
        defaultValue = other.defaultValue;
        miscStuff = other.miscStuff;
        codePage = other.codePage;
        unsignedDT = other.unsignedDT;
        icon = other.icon;
    }

    return *this;
}


QString UserDataType::sizeAsString() const
{
    return size>0?size:QString("N/A");
}

void UserDataType::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement dtElement = doc.createElement("DataType");      // will hold the data in this element

    QDomElement nameElement = doc.createElement("Name");        // the name node
    QDomText nameNode = doc.createTextNode(name);
    nameElement.appendChild(nameNode);
    dtElement.appendChild(nameElement);

    QDomElement dtTypeElement = doc.createElement("Type");      // type (as number)
    QDomText dtTypeNode = doc.createTextNode(QString::number(getType()));
    dtTypeElement.appendChild(dtTypeNode);
    dtElement.appendChild(dtTypeElement);

    QDomElement sqlTypeElement = doc.createElement("SqlType");  // type as SQL
    QDomText sqlTypeNode = doc.createTextNode(getSqlType());
    sqlTypeElement.appendChild(sqlTypeNode);
    dtElement.appendChild(sqlTypeElement);

    QDomElement sizeElement = doc.createElement("Size");  // size
    QDomText sizeNode = doc.createTextNode(getSize());
    sizeElement.appendChild(sizeNode);
    dtElement.appendChild(sizeElement);

    parent.appendChild(dtElement);
}
