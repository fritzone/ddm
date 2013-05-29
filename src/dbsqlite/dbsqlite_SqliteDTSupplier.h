#ifndef SQLITEDTSUPPLIER_H
#define SQLITEDTSUPPLIER_H

#include "db_AbstractDTSupplier.h"

class SqliteDTSupplier : public AbstractDTSupplier
{
public:
    SqliteDTSupplier();

    virtual QList<UserDataType> textTypes();
    virtual QList<UserDataType> numericTypes();
    virtual QList<UserDataType> booleanTypes();
    virtual QList<UserDataType> dateTimeTypes();
    virtual QList<UserDataType> blobTypes();
    virtual QList<UserDataType> miscTypes();
    virtual QList<UserDataType> spatialTypes();

    /**
     * This functions pecifies if the given SQL type has the "SIZE" allowed,
     * ie, if we can write like: column varchar(256) or not ....
     */
    virtual int maximumSize(const QString&);

    virtual QString defaultSqlForGenericType(const QString& genericType);
    virtual bool isValid(const UserDataType* udt);
    virtual QString typeForSqlType(const QString& sqlType);

};

#endif // SQLITEDTSUPPLIER_H
