#ifndef ABSTRACTDTSUPPLIER_H
#define ABSTRACTDTSUPPLIER_H

#include <QList>

#include "core_UserDataType.h"

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
    virtual QList<UserDataType> textTypes() = 0;

    /**
     * Method which will supply the supported numeric types for the specific
     * database engine
     */
    virtual QList<UserDataType> numericTypes() = 0;

    /**
     * Method which will supply the supported boolean types of the DB
     */
    virtual QList<UserDataType> booleanTypes() = 0;

    /**
     * Method which will supply the date/time types of the DB engine
     */
    virtual QList<UserDataType> dateTimeTypes() = 0;

    /**
     * Method which will supply the supported blob types
     */
    virtual QList<UserDataType> blobTypes() = 0;

    /**
     * Method which will supply the supported misc types. This includes
     * for example the ENUM mysql type
     */
    virtual QList<UserDataType> miscTypes() = 0;

    /**
     * Method which will supply the supported Spatial types.
     */
    virtual QList<UserDataType> spatialTypes() = 0;

    /**
     * This functions pecifies if the given SQL type has the "SIZE" allowed,
     * ie, if we can write like: column varchar(256) or not ....
     * Return value:
     * 0   - size is not supported (size txt in GUI will be hidden)
     * > 0 - size is supported (size txt will be shown)
     * -1  - this is an enum type (size txt hidden, values list shown)
     */
    virtual int maximumSize(const QString&) = 0;

    virtual DT_TYPE getDT_TYPE(const QString& usedSql) = 0;

    /**
     * Returns the default SQL type for the given generic type
     * (For example VARCHAR for String/Text, etc...)
     */
    virtual QString defaultSqlForGenericType(const QString& genericType) = 0;

    virtual QString typeForSqlType(const QString& sqlType) = 0;

    QList<UserDataType> getDTList(DT_TYPE dt)
    {
        switch(dt)
        {
        case DT_STRING: return textTypes();
        case DT_NUMERIC: return numericTypes();
        case DT_BOOLEAN: return booleanTypes();
        case DT_BLOB: return blobTypes();
        case DT_DATETIME: return dateTimeTypes();
        case DT_MISC: return miscTypes();
        case DT_SPATIAL: return spatialTypes();
        default: return QList<UserDataType>();
        }
    }

    /**
     * @brief getDefaultForDT return the default datatype for the given group
     * @param dt
     * @return
     */
    virtual UserDataType* getDefaultForDT(DT_TYPE dt) = 0;
};



#endif // ABSTRACTDTSUPPLIER_H
