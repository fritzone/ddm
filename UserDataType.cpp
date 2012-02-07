#include "UserDataType.h"

#include <QComboBox>

UserDataType::UserDataType(const QString& name, const QString& typeString,
                           const QString& _sqlType, const QString& _s,
                           const QString& _defaultValue, const QString& _cp,
                           const QStringList& _mvs, bool unsi, const QString& desc,
                           bool nullable, bool autoInc) :
        DataType(name, DataType::getDT_TYPE(typeString)),
        sqlType(_sqlType),
        size(_s), defaultValue(_defaultValue), miscStuff(_mvs), codePage(_cp),
        unsignedDT(unsi), description(desc), canBeNull(nullable), autoIncrement(autoInc)
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
        description = other.description;
        canBeNull = other.canBeNull;
        autoIncrement = other.autoIncrement;
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

    dtElement.setAttribute("Name", name);
    dtElement.setAttribute("Type", typeAsString());
    dtElement.setAttribute("SqlType", getSqlType());
    dtElement.setAttribute("Size", getSize());
    dtElement.setAttribute("DefaultValue", getDefaultValue());
    dtElement.setAttribute("Codepage", getCodepage());
    dtElement.setAttribute("Unsigned", isUnsigned() );
    dtElement.setAttribute("CanBeNull", canBeNull);
    dtElement.setAttribute("AutoIncrement", autoIncrement);

    {
    QDomElement descElement = doc.createElement("Description");  // description
    QDomText descNode = doc.createTextNode(getDescription());
    descElement.appendChild(descNode);
    dtElement.appendChild(descElement);
    }

    {
    QDomElement valuesElement = doc.createElement("Values");  // Values
    for(int i=0; i<getMiscValues().size(); i++)
    {
        QDomElement valueElement = doc.createElement("Value");  // Values
        QDomText valuesNode = doc.createTextNode(getMiscValues()[i]);
        valueElement.appendChild(valuesNode);
        valuesElement.appendChild(valueElement);
    }

    dtElement.appendChild(valuesElement);
    }


    parent.appendChild(dtElement);
}

QWidget* UserDataType::getDefaultsTableWidget() const
{
//    if(getType() == DT_BOOLEAN)
//    {
//        QComboBox* cmb = new QComboBox(0);
//        cmb->addItem("TRUE");
//        cmb->addItem("FALSE");
//        return cmb;
//    }

    return 0;
}

bool UserDataType::isValid(const QString& v) const
{
    if(type == DT_NUMERIC)
    {
        for(int i=0; i<v.length(); i++)
        {
            QChar c = v.at(i);
            if(!c.isNumber())
            {
                return false;
            }
        }
    }
    return true;
}
