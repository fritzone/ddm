#ifndef ABSTRACTDTSUPPLIER_H
#define ABSTRACTDTSUPPLIER_H

#include <QList>

#include "DataType.h"

class UserDataType;

/**
 * This abstract class is responsible to supply Data Types for each database
 * engine. The implemented mechanism is to supply a list of DataTypes
 * for each database engine.
 */
class AbstractDTSupplier
{
public:
    AbstractDTSupplier(){}
    virtual ~AbstractDTSupplier() {}


    /**
     * Method which will supply the supported text types of the database
     * engine.
     */
    virtual QList<DataType> textTypes() = 0;

    /**
     * Method which will supply the supported numeric types for the specific
     * database engine
     */
    virtual QList<DataType> numericTypes() = 0;

    /**
     * Method which will supply the supported boolean types of the DB
     */
    virtual QList<DataType> booleanTypes() = 0;

    /**
     * Method which will supply the date/time types of the DB engine
     */
    virtual QList<DataType> dateTimeTypes() = 0;

    /**
     * Method which will supply the supported blob types
     */
    virtual QList<DataType> blobTypes() = 0;

    /**
     * Method which will supply the supported misc types. This includes
     * for example the ENUM mysql type
     */
    virtual QList<DataType> miscTypes() = 0;

    /**
     * Method which will supply the supported Spatial types.
     */
    virtual QList<DataType> spatialTypes() = 0;

    /**
     * This functions pecifies if the given SQL type has the "SIZE" allowed,
     * ie, if we can write like: column varchar(256) or not ....
     * Return value:
     * 0   - size is not supported (size txt in GUI will be hidden)
     * > 0 - size is supported (size txt will be shown)
     * -1  - this is an enum type (size txt hidden, values list shown)
     */
    virtual int maximumSize(const QString&) = 0;

    /**
     * Checks if the supplied User data type is valid for this DT supplier
     */
    virtual bool isValid(const UserDataType* udt) = 0;

/*
  Later we will move the SQL type combo box in the "Advanced" section of
  the GUI and introduce a new data type: real and hide the SQL in the
  Data Type creation window. But this is future
  */
    /**
     * Returns the default SQL type for the given generic type
     * (For example VARCHAR for String/Text, etc...)
     */
    virtual QString defaultSqlForGenericType(const QString& genericType) = 0;

    virtual QString typeForSqlType(const QString& sqlType) = 0;

    static QList<DataType> getDTList(AbstractDTSupplier* dts, DataType::DT_TYPE dt)
    {
        switch(dt)
        {
        case DataType::DT_STRING: return dts->textTypes();
        case DataType::DT_NUMERIC: return dts->numericTypes();
        case DataType::DT_BOOLEAN: return dts->booleanTypes();
        case DataType::DT_BLOB: return dts->blobTypes();
        case DataType::DT_DATETIME: return dts->dateTimeTypes();
        case DataType::DT_MISC: return dts->miscTypes();
        case DataType::DT_SPATIAL: return dts->spatialTypes();
        default: return QList<DataType>();
        }
    }
};



#endif // ABSTRACTDTSUPPLIER_H
