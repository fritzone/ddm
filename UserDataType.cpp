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

    QDomElement dtTypeElement = doc.createElement("Type");
    QDomText dtTypeNode = doc.createTextNode(QString::number(getType()));
    dtTypeElement.appendChild(dtTypeNode);

    QDomElement sqlTypeElement = doc.createElement("SqlType");
    QDomText sqlTypeNode = doc.createTextNode(getSqlType());
    sqlTypeElement.appendChild(sqlTypeNode);




    parent.appendChild(dtElement);

}