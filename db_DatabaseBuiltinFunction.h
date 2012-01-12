#ifndef DATABASEBUILTINFUNCTION_H
#define DATABASEBUILTINFUNCTION_H

#include "qbr_OptionTypes.h"
#include "db_DatabaseBuiltinFunctionsParameter.h"
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
    explicit DatabaseBuiltinFunction(QString name, FunctionType type, const UserDataType& returnType, const QString& desc) :
            NamedItem(name), m_type(type), m_returnType(returnType), m_parameters(), m_description(desc)
    {
    }



    explicit DatabaseBuiltinFunction(QString name, FunctionType type, const UserDataType& returnType, DatabaseBuiltinFunctionsParameter p1, const QString& desc) :
            NamedItem(name), m_type(type), m_returnType(returnType), m_parameters(), m_description(desc)
    {
        m_parameters << p1;
    }

    explicit DatabaseBuiltinFunction(QString name, FunctionType type, const UserDataType& returnType, DatabaseBuiltinFunctionsParameter p1,
                                     DatabaseBuiltinFunctionsParameter p2, const QString& desc) : NamedItem(name), m_type(type), m_returnType(returnType), m_parameters(), m_description(desc)
    {
        m_parameters << p1;
        m_parameters << p2;
    }

    explicit DatabaseBuiltinFunction(QString name, FunctionType type, const UserDataType& returnType,
                                     DatabaseBuiltinFunctionsParameter p1,
                                     DatabaseBuiltinFunctionsParameter p2,
                                     DatabaseBuiltinFunctionsParameter p3,
                                     const QString& desc) : NamedItem(name), m_type(type), m_returnType(returnType), m_parameters(), m_description(desc)
    {
        m_parameters << p1;
        m_parameters << p2;
        m_parameters << p3;
    }

    explicit DatabaseBuiltinFunction(QString name, FunctionType type, const UserDataType& returnType, DatabaseBuiltinFunctionsParameter p1,
                                     DatabaseBuiltinFunctionsParameter p2,
                                     DatabaseBuiltinFunctionsParameter p3,
                                     DatabaseBuiltinFunctionsParameter p4,
                                     const QString& desc) : NamedItem(name), m_type(type), m_returnType(returnType), m_parameters(), m_description(desc)
    {
        m_parameters << p1;
        m_parameters << p2;
        m_parameters << p3;
        m_parameters << p4;
    }

    explicit DatabaseBuiltinFunction(QString name, FunctionType type, const UserDataType& returnType, DatabaseBuiltinFunctionsParameter p1,
                                     DatabaseBuiltinFunctionsParameter p2,
                                     DatabaseBuiltinFunctionsParameter p3,
                                     DatabaseBuiltinFunctionsParameter p4,
                                     DatabaseBuiltinFunctionsParameter p5,
                                     const QString& desc) : NamedItem(name), m_type(type), m_returnType(returnType), m_parameters(), m_description(desc)
    {
        m_parameters << p1;
        m_parameters << p2;
        m_parameters << p3;
        m_parameters << p4;
        m_parameters << p5;
    }

    explicit DatabaseBuiltinFunction(QString name, FunctionType type, const UserDataType& returnType, DatabaseBuiltinFunctionsParameter p1,
                                     DatabaseBuiltinFunctionsParameter p2,
                                     DatabaseBuiltinFunctionsParameter p3,
                                     DatabaseBuiltinFunctionsParameter p4,
                                     DatabaseBuiltinFunctionsParameter p5,
                                     DatabaseBuiltinFunctionsParameter p6,
                                     const QString& desc) : NamedItem(name), m_type(type), m_returnType(returnType), m_parameters(), m_description(desc)
    {
        m_parameters << p1;
        m_parameters << p2;
        m_parameters << p3;
        m_parameters << p4;
        m_parameters << p5;
        m_parameters << p6;
    }



    DatabaseBuiltinFunction() : NamedItem("N/A"), m_type(FT_INVALID), m_returnType(), m_parameters(), m_description() {}

    int getParameterCount() const
    {
        return m_parameters.size();
    }

    const QString& getDescription() const
    {
        return m_description;
    }

    FunctionType getType() const
    {
        return m_type;
    }

private:
    FunctionType m_type;
    UserDataType m_returnType;
    QVector<DatabaseBuiltinFunctionsParameter> m_parameters;
    QString m_description;
};

#endif // DATABASEBUILTINFUNCTION_H
