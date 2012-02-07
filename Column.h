#ifndef COLUMN_H
#define COLUMN_H

#include "TreeItem.h"
#include "SerializableElement.h"
#include "CopyableElement.h"
#include "IssueOriginator.h"
#include "NamedItem.h"

#include <QString>

class UserDataType;
class Table;

/**
 * This class holds the necessary data to define a column
 */
class Column : public TreeItem, public SerializableElement, public CopyableElement, public IssueOriginator, public NamedItem
{
public:
    Column(const QString& name, const UserDataType* type, bool pk, bool autoInc);

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

    void setTable(Table* tab)
    {
        m_table = tab;
    }

    virtual QString getFullLocation() const;

    virtual Table* getIssueTable() const
    {
        return getTable();
    }

    Table* getTable() const
    {
        return m_table;
    }

private:
    const UserDataType* m_type;
    bool m_pk;
    bool m_autoIncrement;
    QString m_description;
    Table* m_table;
};

#endif // COLUMN_H
