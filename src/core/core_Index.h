#ifndef INDEX_H
#define INDEX_H

#include "TreeItem.h"
#include "core_NamedItem.h"
#include "core_SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "ObjectWithSpInstances.h"
#include "core_CloneableElement.h"
#include "core_VersionElement.h"

#include <QVector>
#include <QString>

class Column;
class Table;

class Index : public TreeItem,
        public SerializableElement,
        public NamedItem,
        public VersionElement,
        public ObjectWithSpInstances,
        public CloneableElement
{

    /**
     * @brief The ColumnAndOrder struct holds a column and the specified
     * sort order in which the column will be sorted in the index.
     */
    struct ColumnAndSortOrder
    {
        // the column
        const Column* c;

        // the order
        QString order;
    };

public:

    Index(const QString& name, Table* tab, const QString& uid, Version *v);

    void addColumn(const Column* column, const QString& order);

    void addColumn(const Column* column, const QString& order, int pos);

    bool hasColumn(const Column*) const;

    void resetColumns();

    QVector<const Column*> getColumns() const;

    void addSpToColumn(const Column* c, const QString& db, SpInstance* spi);

    Table* getTable() const
    {
        return m_owner;
    }

    QMap<QString, QVector<SpInstance*> > getSpsOfColumn(const Column*) const;

    SpInstance* getSpiOfColumnForSpecificRole(const Column* c, const QString& role, const QString& db) const;

    QString getOrderForColumn(const Column*) const;

    void finalizeCloning(Table*, Index *source);

    CloneableElement* clone(Version *sourceVersion, Version *targetVersion);

    virtual QUuid getClassUid() const;

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

private:

    // this is the table which has this index
    Table* m_owner;

    // this is here to just to keep the order of the columns
    QVector<ColumnAndSortOrder*> m_columns;

    // The following is to be interpreted as:
    // for each column there is a map, mapping a database name to a vector of SP instances from that specific database
    QMap<QString, QMap<QString, QVector<SpInstance*> > > m_columnsWithSpInstances;

};

#endif // INDEX_H
