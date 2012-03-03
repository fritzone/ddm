#ifndef COLUMN_H
#define COLUMN_H

#include "TreeItem.h"
#include "SerializableElement.h"
#include "CopyableElement.h"
#include "IssueOriginator.h"
#include "NamedItem.h"
#include "core_ItemWithDescription.h"
#include "core_ObjectWithUid.h"

#include <QString>

class UserDataType;
class Table;

/**
 * This class holds the necessary data to define a column
 */
class Column : virtual public TreeItem, virtual public SerializableElement, virtual public CopyableElement,
        virtual public IssueOriginator, virtual public NamedItem, virtual public ItemWithDescription,
        virtual public ObjectWithUid
{
public:
    Column(const QString& uid, const QString& name, const UserDataType* type, bool pk, bool autoInc);

    const UserDataType* getDataType() const;

    bool isPk() const;

    void setDataType(const UserDataType* dt);

    void setPk(bool pk);

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

    virtual QUuid getClassUid() const;

private:
    const UserDataType* m_type;
    bool m_pk;
    bool m_autoIncrement;
    Table* m_table;
};

#endif // COLUMN_H
