#ifndef DATABASEBUILTINFUNCTIONSPARAMETER_H
#define DATABASEBUILTINFUNCTIONSPARAMETER_H

#include <Qt>

#include "qbr_OptionTypes.h"
#include "UserDataType.h"

/**
 * This class describes a parameter that goes into a DatabaseBuiltinFunction,
 * such as type, mandatoriness, etc...
 * When we are building a query the class DatabaseFunctionInstantiationComponent holds
 * the values in forms of SingleExpressionQueryComponents ...
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
