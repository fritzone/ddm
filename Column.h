#ifndef COLUMN_H
#define COLUMN_H

#include "TreeItem.h"
#include "SerializableElement.h"
#include "CopyableElement.h"

#include <QString>

class UserDataType;

/**
 * This class holds the necessary data to define a column
 */
class Column : virtual public TreeItem, virtual public SerializableElement, virtual public CopyableElement
{
public:
    Column(const QString& name, const UserDataType* type, bool, bool );

    const QString& getName() const;

    const UserDataType* getDataType() const;

    bool isPk() const;

    void setName(const QString& name);

    void setDataType(const UserDataType* dt);

    void setPk(bool pk);

    void setDescription(const QString& desc);

    QString getDescription() const;

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    bool hasAutoIncrement() const;

    void setAutoIncrement(bool a);

    virtual void copy();

private:
    QString m_name;
    const UserDataType* m_type;
    bool m_pk;
    bool m_autoIncrement;
    QString m_description;
};

#endif // COLUMN_H
