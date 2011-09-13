#ifndef DATATYPE_H
#define DATATYPE_H

#include <QString>
#include <QIcon>

#include "strings.h"

/**
 * This is a base class representing a data type
 */
class   DataType
{
public:
    enum DT_TYPE
    {
        DT_STRING = 0,
        DT_NUMERIC,
        DT_BOOLEAN,
        DT_DATETIME,
        DT_BLOB,
        DT_MISC,
        DT_SPATIAL,
        DT_INVALID,
        DT_GENERIC,
    } ;

    /**
     * Constructor
     */
    DataType(const QString& _name, DT_TYPE _type);

    DataType() : name(""), type(DT_INVALID)
    {}

    DataType(const DataType& other):name(other.name), type(other.type)
    {}

    virtual ~DataType() {}

    QString getName() const
    {
        return name;
    }

    DT_TYPE getType() const
    {
        return type;
    }

    QString typeAsString() const;

    static DT_TYPE getDT_TYPE(const QString& typeString);

    static QString getIconString(DT_TYPE dt)
    {
        switch(dt)
        {
            case DT_STRING: return strDtTextIcon;
            case DT_NUMERIC: return strDtNumericIcon;
            case DT_BOOLEAN: return strDtBoolIcon;
            case DT_BLOB: return strDtBlobIcon;
            case DT_DATETIME: return strDtDateTimeIcon;
            case DT_MISC: return strDtMiscIcon;
            case DT_SPATIAL: return strDtSpatialIcon;
            default: return "";
        }
    }

    static const QIcon& getIcon(DT_TYPE dt);

    void setName(const QString& s)
    {
        name = s;
    }

protected:
    // the name of the data type
    QString name;

    // the type of this data type
    DT_TYPE type;

};

#endif // DATATYPE_H
