#ifndef DATABASEBUILTINFUNCTIONSPARAMETER_H
#define DATABASEBUILTINFUNCTIONSPARAMETER_H

#include <Qt>

#include "OptionsType.h"

/**
 * This class describes a parameter that goes into a DatabaseBuiltinFunction,
 * such as type, mandatoriness, etc...
 * Also, this is used for storing the value of it, populated from the query
 */
class DatabaseBuiltinFunctionsParameter
{
public:
    DatabaseBuiltinFunctionsParameter(QString name, FunctionReturnType type, bool mandatory) : m_name(name), m_type(type), m_mandatory(mandatory) {}
    DatabaseBuiltinFunctionsParameter() {}

private:
    QString m_name;
    FunctionReturnType m_type;
    bool m_mandatory;
};

#endif // DATABASEBUILTINFUNCTIONSPARAMETER_H
