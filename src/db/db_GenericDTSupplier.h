#ifndef DB_GENERICDTSUPPLIER_H
#define DB_GENERICDTSUPPLIER_H

#include "db_AbstractDTSupplier.h"

#include <QMap>

class GenericDatabaseType;

class GenericDTSupplier : public AbstractDTSupplier
{
public:
    GenericDTSupplier(const QVector<GenericDatabaseType*> dbtypes);

    virtual QList<UserDataType> textTypes()
    {
        return m_textTypes;
    }

    virtual QList<UserDataType> numericTypes()
    {
        return m_numericTypes;
    }

    virtual QList<UserDataType> booleanTypes()
    {
        return m_booleanTypes;
    }

    virtual QList<UserDataType> dateTimeTypes()
    {
        return m_dateTypes;
    }

    virtual QList<UserDataType> blobTypes()
    {
        return m_blobTypes;
    }

    virtual QList<UserDataType> miscTypes()
    {
        return m_miscTypes;
    }

    virtual QList<UserDataType> spatialTypes()
    {
        return m_spatialTypes;
    }

    virtual int maximumSize(const QString& dt)
    {
        if(m_maximumSizes.contains(dt))
        {
            return m_maximumSizes[dt];
        }

        return 0;
    }

    virtual QString defaultSqlForGenericType(const QString& genericType)
    {
        if(m_defaultTypes.contains(genericType))
        {
            return m_defaultTypes[genericType];
        }

        return "";
    }

    virtual QString typeForSqlType(const QString& sqlType)
    {
        if(m_dataTypesOfType.contains(sqlType.toUpper()))
        {
            return m_dataTypesOfType[sqlType];
        }

        return "";
    }

    virtual DT_TYPE getDT_TYPE(const QString& usedSql)
    {
        for(int i=0; i<m_dtTypes.keys().size(); i++)
        {
            if(usedSql.toUpper().startsWith(m_dtTypes.keys().at(i)))
            {
                return m_dtTypes[m_dtTypes.keys().at(i)];
            }
        }

        return DT_INVALID;
    }

private:

    // the various types of the database
    QList<UserDataType> m_textTypes;
    QList<UserDataType> m_numericTypes;
    QList<UserDataType> m_booleanTypes;
    QList<UserDataType> m_dateTypes;
    QList<UserDataType> m_blobTypes;
    QList<UserDataType> m_miscTypes;
    QList<UserDataType> m_spatialTypes;

    // contains the maximum size for the types that were specified.
    QMap<QString, int> m_maximumSizes;

    // a map for the DT types for the specified database types
    QMap<QString, DT_TYPE> m_dtTypes;

    // the default types for the defineable User Data Types
    QMap<QString, QString> m_defaultTypes;

    // the type of each data type (such as Numeric, Date/Time, etc...
    QMap<QString, QString> m_dataTypesOfType;


};

#endif // DB_GENERICDTSUPPLIER_H
