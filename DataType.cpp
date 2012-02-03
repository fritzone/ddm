#include "DataType.h"
#include "strings.h"
#include "IconFactory.h"

#include <QString>

/**
 * Creates a new DataType object
 */
DataType::DataType(const QString& _name, DT_TYPE _type) : name(_name),
type(_type)
{
}

/**
 * Returns the DT_TYPE for the given string (taken from the combo box)
 */
DataType::DT_TYPE DataType::getDT_TYPE(const QString& typeString)
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

QIcon DataType::getIcon(DT_TYPE dt)
{
    switch(dt)
    {
    case DT_STRING: return IconFactory::getStringDataTypeIcon();
    case DT_NUMERIC: return IconFactory::getIntDataTypeIcon();
    case DT_BOOLEAN: return IconFactory::getBoolDataTypeIcon();
    case DT_BLOB: return IconFactory::getBlobDataTypeIcon();
    case DT_DATETIME: return IconFactory::getDateTimeDataTypeIcon();
    case DT_MISC: return IconFactory::getMiscDataTypeIcon();
    case DT_SPATIAL: return IconFactory::getSpatialDataTypeIcon();
    case DT_INVALID:
    case DT_GENERIC:
    default:
        return IconFactory::getEmptyIcon();
    }

    return IconFactory::getEmptyIcon();
}

QString DataType::typeAsString() const
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

