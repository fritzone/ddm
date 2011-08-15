#ifndef DATABASEBUILTINFUNCTION_H
#define DATABASEBUILTINFUNCTION_H

#include "OptionsType.h"
#include "DatabaseBuiltinFunctionsParameter.h"
#include "NamedItem.h"
#include "UserDataType.h"

#include <QVector>

/**
 * This class holds the descriptive information of a function that is built in the database, such as name
 * parameters and their type, return type, etc...
 */
class DatabaseBuiltinFunction : public NamedItem
{
public:
    explicit DatabaseBuiltinFunction(QString name, FunctionType type, const UserDataType& returnType, DatabaseBuiltinFunctionsParameter p1) : NamedItem(name),
        m_type(type), m_returnType(returnType), m_parameters()
    {
        m_parameters << p1;
    }

    DatabaseBuiltinFunction() : m_type(FT_INVALID), m_returnType(), m_parameters() {}

    int getParameterCount()
    {
        return m_parameters.size();
    }

private:
    FunctionType m_type;
    UserDataType m_returnType;
    QVector<DatabaseBuiltinFunctionsParameter> m_parameters;
};

#endif // DATABASEBUILTINFUNCTION_H
