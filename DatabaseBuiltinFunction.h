#ifndef DATABASEBUILTINFUNCTION_H
#define DATABASEBUILTINFUNCTION_H

#include "OptionsType.h"
#include "DatabaseBuiltinFunctionsParameter.h"

#include <QVector>

/**
 * This class holds the descriptive information of a function that is built in the database, such as name
 * parameters and their type, return type, etc...
 */
class DatabaseBuiltinFunction
{
public:
    DatabaseBuiltinFunction(QString name, FunctionType type, FunctionReturnType frt, DatabaseBuiltinFunctionsParameter p1) :
        m_name(name), m_type(type), m_frt(frt), m_parameters()
    {
        m_parameters << p1;
    }

    DatabaseBuiltinFunction(){}

    const QString& getName() const
    {
        return m_name;
    }

private:
    QString m_name;
    FunctionType m_type;
    FunctionReturnType m_frt;
    QVector<DatabaseBuiltinFunctionsParameter> m_parameters;
};

#endif // DATABASEBUILTINFUNCTION_H
