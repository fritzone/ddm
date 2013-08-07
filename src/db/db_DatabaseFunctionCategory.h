#ifndef DB_DATABASEFUNCTIONCATEGORY_H
#define DB_DATABASEFUNCTIONCATEGORY_H

#include <QString>
#include "core_NamedItem.h"

/**
 * @brief The DatabaseFunctionCategory class represents a function category
 * as read from the database repository
 */
class DatabaseFunctionCategory : public NamedItem
{
public:

    DatabaseFunctionCategory() : NamedItem("invalid"), m_idName("invalid"), m_id(-1), m_isAggregate(false)
    {}

    DatabaseFunctionCategory(const QString& name, const QString& idName, int id, bool isAggregate):
        NamedItem(name),
        m_idName(idName), m_id(id), m_isAggregate(isAggregate)
    {}

    int getId() const
    {
        return m_id;
    }

    void setId(int id)
    {
        m_id = id;
    }

    QString getIdName() const
    {
        return m_idName;
    }

    void setIdName(const QString &idName)
    {
        m_idName = idName;
    }

    bool isAggregate() const
    {
        return m_isAggregate;
    }

private:

    QString m_idName;
    int m_id;
    bool m_isAggregate;
};



#endif // DB_DATABASEFUNCTIONCATEGORY_H
