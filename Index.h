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

    struct ColumnAndOrder
    {
        const Column* c;
        QString order;
    };

public:

    Index(const QString& name, Table* tab, const QString& uid);

    void addColumn(const Column* column, const QString& order);

    void addColumn(const Column* column, const QString& order, int pos);

    bool hasColumn(const Column*) const;

    void resetColumns();

    QVector<const Column*> getColumns() const
    {
        return m_columnsWithSpInstances.keys().toVector();
    }

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    Table* getOwner() const
    {
        return m_owner;
    }

    virtual QUuid getClassUid() const;

private:

    Table* m_owner;

    // this is here to just to keep the order of the columns
    QVector<ColumnAndOrder*> m_columns;

    // The following is to be interpreted as:
    // for each column there is a map, mapping a database name to a vector of SP instances from that specific database
    QMap< const Column*, QMap<QString, QVector<SpInstance*> > > m_columnsWithSpInstances;

};

#endif // INDEX_H
