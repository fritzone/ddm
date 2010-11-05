#ifndef COLUMN_H
#define COLUMN_H

#include "TreeItem.h"
#include "SerializableElement.h"
#include "CopyableElement.h"
#include "NamedItem.h"

#include <QString>

class UserDataType;

/**
 * This class holds the necessary data to define a column
 */
class Column : public TreeItem, public SerializableElement, public CopyableElement, public NamedItem
{
public:
    Column(const QString& name, const UserDataType* type, bool, bool );

    const UserDataType* getDataType() const;

    bool isPk() const;

    void setDataType(const UserDataType* dt);

    void setPk(bool pk);

    void setDescription(const QString& desc);

    QString getDescription() const;

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    bool hasAutoIncrement() const;

    void setAutoIncrement(bool a);

    virtual void copy();

private:
    const UserDataType* m_type;
    bool m_pk;
    bool m_autoIncrement;
    QString m_description;
};

#endif // COLUMN_H
