#include "core_UserDataType.h"
#include "strings.h"
#include "uids.h"

#include <QComboBox>

UserDataType::UserDataType(const QString& name, const QString& typeString,
                           const QString& _sqlType, const QString& _s,
                           const QString& _defaultValue, const QStringList &_mvs,
                           bool unsi, const QString &desc, bool nullable, const QString& uid) :
        NamedItem(name),
        TreeItem(),
        SerializableElement(),
        ObjectWithUid(uid),
        CloneableElement(),
        sqlType(_sqlType),
        size(_s), defaultValue(_defaultValue), miscStuff(_mvs),
        unsignedDT(unsi), description(desc), canBeNull(nullable), m_type(getDT_TYPE(typeString))
{
}

UserDataType& UserDataType::operator = (const UserDataType& other)
{
    if(this != &other)
    {
        setName(other.getName());
        m_type = other.m_type;
        sqlType = other.sqlType;
        size = other.size;
        defaultValue = other.defaultValue;
        miscStuff = other.miscStuff;
        unsignedDT = other.unsignedDT;
        description = other.description;
        canBeNull = other.canBeNull;
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

    dtElement.setAttribute("Name", getName());
    dtElement.setAttribute("Type", typeAsString());
    dtElement.setAttribute("SqlType", getSqlType());
    dtElement.setAttribute("Size", getSize());
    dtElement.setAttribute("DefaultValue", getDefaultValue());
    dtElement.setAttribute("Unsigned", isUnsigned() );
    dtElement.setAttribute("CanBeNull", canBeNull);
    dtElement.setAttribute("uid", getObjectUid());
    dtElement.setAttribute("class-uid", getClassUid().toString());
    dtElement.setAttribute("source-uid", getSourceUid());

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
    if(m_type == DT_NUMERIC)
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

QString UserDataType::typeAsString() const
{
    switch(m_type)
    {
    case DT_STRING: return strTextString;
    case DT_NUMERIC: return strNumeric;
    case DT_BOOLEAN: return strBoolean;
    case DT_BLOB: return strBlob;
    case DT_DATETIME: return strDateTime;
    case DT_MISC: return strMisc;
    case DT_SPATIAL: return strSpatial;
    case DT_GENERIC:
    case DT_INVALID:
    default:
        return strNA;
    }
    return QString("");
}

bool UserDataType::supportsAutoIncrement() const
{
    if(m_type == DT_NUMERIC) return true;
    return false;
}


/**
 * Returns the DT_TYPE for the given string (taken from the combo box)
 */
DT_TYPE UserDataType::getDT_TYPE(const QString& typeString)
{
    if(typeString == strTextString) return DT_STRING;
    if(typeString == strNumeric) return DT_NUMERIC;
    if(typeString == strBlob) return DT_BLOB;
    if(typeString == strBoolean) return DT_BOOLEAN;
    if(typeString == strMisc) return DT_MISC;
    if(typeString == strDateTime) return DT_DATETIME;
    if(typeString == strSpatial) return DT_SPATIAL;

    return DT_INVALID;
}

CloneableElement* UserDataType::clone(Version */*sourceVersion*/, Version */*targetVersion*/)
{
    UserDataType* newUdt = new UserDataType(getName(), getType(), QUuid::createUuid().toString());
    newUdt->size = this->size;
    newUdt->sqlType = this->sqlType;
    newUdt->defaultValue = this->defaultValue;
    newUdt->miscStuff = this->miscStuff;
    newUdt->unsignedDT = this->unsignedDT;
    newUdt->description = this->description;
    newUdt->canBeNull = this->canBeNull;

    newUdt->setSourceUid(getObjectUid());

    return newUdt;
}

QUuid UserDataType::getClassUid() const
{
    switch(m_type)
    {
    case DT_STRING: return uidStringDT;
    case DT_NUMERIC: return uidNumericDT;
    case DT_BOOLEAN: return uidBooleanDT;
    case DT_BLOB: return uidBlobDT;
    case DT_DATETIME: return uidDateTimeDT;
    case DT_MISC: return uidMiscDT;
    case DT_SPATIAL: return uidSpatialDT;
    case DT_GENERIC:
    case DT_INVALID:
    default:
       return nullUid;
    }
}

void UserDataType::updateGui()
{
    getLocation()->setIcon(0, IconFactory::getLockedIconForDataType(m_type));
    TreeItem::updateGui();
}
