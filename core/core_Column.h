#ifndef COLUMN_H
#define COLUMN_H

#include "TreeItem.h"
#include "SerializableElement.h"
#include "CopyableElement.h"
#include "IssueOriginator.h"
#include "NamedItem.h"
#include "core_ItemWithDescription.h"
#include "core_ObjectWithUid.h"
#include "ObjectWithSpInstances.h"
#include "core_CloneableElement.h"

#include <QString>

class UserDataType;
class Version;
class Table;

/**
 * This class holds the necessary data to define a column
 */
class Column :
        public SerializableElement,
        public CopyableElement,
        public IssueOriginator,
        public NamedItem,
        public ItemWithDescription,
        public ObjectWithUid,
        public ObjectWithSpInstances,
        public CloneableElement,
        public TreeItem
{
public:
    Column(const QString& uid, const QString& name, const UserDataType* type, bool pk, Version *v);

    const UserDataType* getDataType() const;

    bool isPk() const;

    void setDataType(const UserDataType* dt);

    void setPk(bool pk);

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

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

    CloneableElement* clone(Version *sourceVersion, Version *targetVersion);

private:

    const UserDataType* m_type;
    bool m_pk;
    Table* m_table;
};

#endif // COLUMN_H
