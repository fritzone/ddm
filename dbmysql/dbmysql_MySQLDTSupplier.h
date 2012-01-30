#ifndef MYSQLDTSUPPLIER_H
#define MYSQLDTSUPPLIER_H

#include "db_AbstractDTSupplier.h"

class MySQLDTSupplier : public AbstractDTSupplier
{
public:
    MySQLDTSupplier();

    virtual QList<DataType> textTypes();
    virtual QList<DataType> numericTypes();
    virtual QList<DataType> booleanTypes();
    virtual QList<DataType> dateTimeTypes();
    virtual QList<DataType> blobTypes();
    virtual QList<DataType> miscTypes();
    virtual QList<DataType> spatialTypes();

    /**
     * This functions pecifies if the given SQL type has the "SIZE" allowed,
     * ie, if we can write like: column varchar(256) or not ....
     */
    virtual int maximumSize(const QString&);

    virtual QString defaultSqlForGenericType(const QString& genericType);
    virtual bool isValid(const UserDataType* udt);
    virtual QString typeForSqlType(const QString& sqlType);

};

#endif // MYSQLDTSUPPLIER_H
