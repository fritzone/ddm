#ifndef COLUMN_H
#define COLUMN_H

#include "TreeItem.h"
#include "SerializableElement.h"

#include <QString>

class UserDataType;

/**
 * This class holds the necessary data to define a column
 */
class Column : virtual public TreeItem, public SerializableElement
{
public:
    Column(const QString& name, const UserDataType* type, bool pk);

    const QString& getName() const
    {
        return m_name;
    }

    const UserDataType* getDataType() const
    {
        return m_type;
    }

    bool isPk() const
    {
        return m_pk;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }

    void setDataType(const UserDataType* dt)
    {
        m_type = dt;
    }

    void setPk(bool pk)
    {
        m_pk = pk;
    }

    void setDescription(const QString& desc)
    {
        m_description = desc;
    }

    QString getDescription() const
    {
        return m_description;
    }

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

private:
    QString m_name;
    const UserDataType* m_type;
    bool m_pk;
    QString m_description;
};

#endif // COLUMN_H
