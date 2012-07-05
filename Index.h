#ifndef INDEX_H
#define INDEX_H

#include "TreeItem.h"
#include "NamedItem.h"
#include "SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "ObjectWithSpInstances.h"
#include "core_CloneableElement.h"

#include <QVector>
#include <QString>

class Column;
class Table;

class Index : public TreeItem, public SerializableElement, public NamedItem, public ObjectWithUid, public ObjectWithSpInstances, public CloneableElement
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

    QVector<const Column*> getColumns() const;

    void addSpToColumn(const Column* c, const QString& db, SpInstance* spi);

    Table* getOwner() const
    {
        return m_owner;
    }

    virtual QUuid getClassUid() const;

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    QMap<QString, QVector<SpInstance*> > getSpsOfColumn(const Column*) const;

    SpInstance* getSpiOfColumnForSpecificRole(const Column* c, const QString& role, const QString& db) const;

    QString getOrderForColumn(const Column*) const;

    CloneableElement* clone(Version *sourceVersion, Version *targetVersion);

    void finalizeCloning(Table*, Index *source);

private:

    Table* m_owner;

    // this is here to just to keep the order of the columns
    QVector<ColumnAndOrder*> m_columns;

    // The following is to be interpreted as:
    // for each column there is a map, mapping a database name to a vector of SP instances from that specific database
    QMap<QString, QMap<QString, QVector<SpInstance*> > > m_columnsWithSpInstances;

};

#endif // INDEX_H
