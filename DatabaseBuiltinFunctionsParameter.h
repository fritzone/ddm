#ifndef DATABASEBUILTINFUNCTIONSPARAMETER_H
#define DATABASEBUILTINFUNCTIONSPARAMETER_H

#include <Qt>

#include "OptionsType.h"
#include "UserDataType.h"

/**
 * This class describes a parameter that goes into a DatabaseBuiltinFunction,
 * such as type, mandatoriness, etc...
 * Also, this is used for storing the value of it, populated from the query
 */
class DatabaseBuiltinFunctionsParameter
{
public:
    explicit DatabaseBuiltinFunctionsParameter(QString name, const UserDataType& type, bool mandatory) : m_name(name), m_type(type), m_mandatory(mandatory) {}
    DatabaseBuiltinFunctionsParameter() : m_name("invalid"), m_type(), m_mandatory(false) {}
private:
    QString m_name;
    UserDataType m_type;
    bool m_mandatory;
};

#endif // DATABASEBUILTINFUNCTIONSPARAMETER_H
