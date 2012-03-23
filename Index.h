#ifndef INDEX_H
#define INDEX_H

#include "TreeItem.h"
#include "NamedItem.h"
#include "SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "ObjectWithSpInstances.h"

#include <QVector>
#include <QString>

class Column;
class Table;

class Index : public TreeItem, public SerializableElement, public NamedItem, public ObjectWithUid, public ObjectWithSpInstances
{
public:

    Index(const QString& name, Table* tab, const QString& uid);

    void addColumn(const Column* column);

    void addColumn(const Column* column, int pos);

    bool hasColumn(const Column*) const;

    void resetColumns();

    const QVector<const Column*>& getColumns() const
    {
        return m_columns;
    }

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    Table* getOwner() const
    {
        return m_owner;
    }

    virtual QUuid getClassUid() const;

private:

    Table* m_owner;

    QVector<const Column*> m_columns;
};

#endif // INDEX_H
