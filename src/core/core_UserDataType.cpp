#include "core_UserDataType.h"
#include "ddm_strings.h"
#include "uids.h"
#include "ContextMenuCollection.h"

#include <QComboBox>

UserDataType::UserDataType(const QString& name, const QString& typeString,
                           const QString& _sqlType, const QString& _s,
                           const QString& _defaultValue, const QStringList &_mvs,
                           bool unsi, const QString &desc, bool nullable, const QString& uid, Version* v) :
        NamedItem(name),
        SerializableElement(),
        VersionElement(uid, v),
        CloneableElement(),
        TreeItem(),
        m_sqlType(_sqlType),
        m_size(_s), m_defaultValue(_defaultValue), m_miscStuff(_mvs),
        m_unsigned(unsi), m_description(desc), m_canBeNull(nullable), m_type(getDT_TYPE(typeString))
{
}

UserDataType& UserDataType::operator = (const UserDataType& other)
{
    if(this != &other)
    {
        setName(other.getName());
        m_type = other.m_type;
        m_sqlType = other.m_sqlType;
        m_size = other.m_size;
        m_defaultValue = other.m_defaultValue;
        m_miscStuff = other.m_miscStuff;
        m_unsigned = other.m_unsigned;
        m_description = other.m_description;
        m_canBeNull = other.m_canBeNull;
    }

    return *this;
}

QString UserDataType::sizeAsString() const
{
    return m_size>0?m_size:QString("N/A");
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
    dtElement.setAttribute("CanBeNull", m_canBeNull);
    dtElement.setAttribute("uid", getObjectUid().toString());
    dtElement.setAttribute("class-uid", getClassUid().toString());
    dtElement.setAttribute("source-uid", getSourceUid().toString());
    dtElement.setAttribute("locked", lockState() == LockableElement::LOCKED);
    dtElement.setAttribute("was-locked", wasLocked());
    dtElement.setAttribute("finalSql", sqlAsString());

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

QString UserDataType::getTypeStringFromDT_TYPE(DT_TYPE type)
{
    switch(type)
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

QString UserDataType::typeAsString() const
{
    return getTypeStringFromDT_TYPE(m_type);
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

DT_TYPE UserDataType::toDtType(int a)
{
    switch(a)
    {
    case 0: return DT_STRING;
    case 1: return DT_NUMERIC;
    case 2: return DT_BOOLEAN;
    case 3: return DT_DATETIME;
    case 4: return DT_BLOB;
    case 5: return DT_MISC;
    case 6: return DT_SPATIAL;
    default:
       return DT_INVALID;
    }

    return DT_INVALID;
}

CloneableElement* UserDataType::clone(Version* /*sourceVersion*/, Version *targetVersion)
{
    UserDataType* newUdt = new UserDataType(getName(), getType(), QUuid::createUuid().toString(), targetVersion);
    newUdt->m_size = this->m_size;
    newUdt->m_sqlType = this->m_sqlType;
    newUdt->m_defaultValue = this->m_defaultValue;
    newUdt->m_miscStuff = this->m_miscStuff;
    newUdt->m_unsigned = this->m_unsigned;
    newUdt->m_description = this->m_description;
    newUdt->m_canBeNull = this->m_canBeNull;

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
    if(lockState() == LockableElement::LOCKED)
    {
        getLocation()->setIcon(0, IconFactory::getLockedIconForDataType(m_type));
        getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getUnlockDataTypePopupMenu());
    }
    else
    if(lockState() == LockableElement::FINAL_LOCK)
    {
        getLocation()->setIcon(0, IconFactory::getFinalLockedIconForDataType(m_type));
        getLocation()->setPopupMenu(0);
    }
    else
    {
        if(wasLocked())
        {
            getLocation()->setIcon(0, IconFactory::getIconForDataType(m_type));
            getLocation()->setPopupMenu(ContextMenuCollection::getInstance()->getRelockDataTypePopupMenu());
        }
    }
    TreeItem::updateGui();
}
