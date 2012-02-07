#ifndef DATATYPE_H
#define DATATYPE_H

#include <QString>

#include "enums.h"
/**
 * This is a base class representing a data type
 */
class DataType
{
public:

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

    void setName(const QString& s)
    {
        name = s;
    }

    bool supportsAutoIncrement() const;

protected:
    // the name of the data type
    QString name;

    // the type of this data type
    DT_TYPE type;

};

#endif // DATATYPE_H
