#include "DataType.h"
#include "strings.h"

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

const QIcon& DataType::getIcon(DT_TYPE dt)
{
    static QIcon stringIcon(DataType::getIconString(DT_STRING));
    static QIcon numericIcon(DataType::getIconString(DT_NUMERIC));
    static QIcon boolIcon(DataType::getIconString(DT_BOOLEAN));
    static QIcon blobIcon(DataType::getIconString(DT_BLOB));
    static QIcon datetimeIcon(DataType::getIconString(DT_DATETIME));
    static QIcon miscIcon(DataType::getIconString(DT_MISC));
    static QIcon spatialIcon(DataType::getIconString(DT_SPATIAL));
    static QIcon emptyIcon ("");

    switch(dt)
    {
    case DT_STRING: return stringIcon;
    case DT_NUMERIC: return numericIcon;
    case DT_BOOLEAN: return boolIcon;
    case DT_BLOB: return blobIcon;
    case DT_DATETIME: return datetimeIcon;
    case DT_MISC: return miscIcon;
    case DT_INVALID: return emptyIcon;
    case DT_SPATIAL: return spatialIcon;
    }

    return emptyIcon;
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
    case DT_INVALID: return strNA;
    }
    return QString("");
}

