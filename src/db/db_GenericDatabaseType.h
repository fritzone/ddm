#ifndef DB_GENERICDATABASETYPE_H
#define DB_GENERICDATABASETYPE_H

#include "core_enums.h"
#include "core_UserDataType.h"

#include <QString>

/**
 * @brief The GenericDatabaseType class represents a data type which is provided
 * by the database engine.
 */
class GenericDatabaseType
{

public:
    /**
     * @brief GenericDatabaseType
     * @param name
     * @param type
     * @param maxSize
     * @param isDefault
     */
    GenericDatabaseType(const QString& name, DT_TYPE type,
                        int maxSize = 0,
                        bool isDefault = false) : m_name(name), m_type(type),
        m_maxSize(maxSize), m_default(isDefault),
        m_genericType(UserDataType::getTypeStringFromDT_TYPE(type))
    {
    }

private:

    // the name of the type (usually SQL, such as CHAR)
    QString m_name;

    // to which ddm type this binds
    DT_TYPE m_type;

    // the maximum allowed size for this
    int m_maxSize;

    // if this is the default data type for its given category
    bool m_default;

    // the ddm type to which this maps
    QString m_genericType;
};

#endif // DB_GENERICDATABASETYPE_H
