#ifndef DATABASEBUILTINFUNCTION_H
#define DATABASEBUILTINFUNCTION_H

#include "core_NamedItem.h"
#include "core_UserDataType.h"

#include "db_DatabaseBuiltinFunctionsParameter.h"
#include "db_DatabaseFunctionCategory.h"

#include <QVector>

/**
 * This class holds the descriptive information of a function that is built in the database, such as name
 * parameters and their type, return type, etc...
 */
class DatabaseBuiltinFunction : public NamedItem
{
public:
    explicit DatabaseBuiltinFunction() : NamedItem("invalid"),
        m_type(),
        m_returnType("return", DT_INVALID, nullUid, Workspace::getInstance()->currentProject()->getWorkingVersion()),
        m_parameters(),
        m_description("")
    {}

    explicit DatabaseBuiltinFunction(QString name, DatabaseFunctionCategory type, const UserDataType& returnType, const QVector<DatabaseBuiltinFunctionsParameter>& p1, const QString& desc) :
            NamedItem(name),
            m_type(type),
            m_returnType(returnType),
            m_parameters(),
            m_description(desc)
    {
        m_parameters = p1;
    }

    int getParameterCount() const
    {
        return m_parameters.size();
    }

    const QString& getDescription() const
    {
        return m_description;
    }

    const DatabaseFunctionCategory& getType() const
    {
        return m_type;
    }

    QString getNiceName() const
    {
        QString a = m_name.mid(1).toUpper();
        return a;
    }

private:
    DatabaseFunctionCategory m_type;
    UserDataType m_returnType;
    QVector<DatabaseBuiltinFunctionsParameter> m_parameters;
    QString m_description;
};

#endif // DATABASEBUILTINFUNCTION_H
