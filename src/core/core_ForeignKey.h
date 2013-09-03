#ifndef FOREIGNKEY_H
#define FOREIGNKEY_H

#include "TreeItem.h"
#include "core_SerializableElement.h"
#include "core_Column.h"
#include "core_NamedItem.h"
#include "core_Table.h"
#include "core_VersionElement.h"

class Version;

class ForeignKey : public TreeItem, public SerializableElement, public NamedItem, public VersionElement
{

public:

    class TableIndexTempStructure
    {
    public:
        QString tabName;
        QString indexName;
    };

    /**
     * Association between a foreign table column and a local table column.
     */
    class ColumnAssociation : public TreeItem
    {
    public:

        ColumnAssociation(const Table* foreignTable, const Column* foreignColumn, const Table* localTable, const Column* localColumn):
                m_foreignTable(foreignTable), m_localTable(localTable), m_foreignColumn(foreignColumn), m_localColumn(localColumn),
                m_sforeignTable(foreignTable->getName()), m_slocalTable(localTable->getName()), m_sforeignColumn(foreignColumn->getName()), m_slocalColumn(localColumn->getName())
        {}

        ColumnAssociation(QString foreignTable, QString foreignColumn, QString localTable, QString localColumn):
                m_foreignTable(0), m_localTable(0), m_foreignColumn(0), m_localColumn(0),
                m_sforeignTable(foreignTable), m_slocalTable(localTable), m_sforeignColumn(foreignColumn), m_slocalColumn(localColumn)
        {}

        const Table* getForeignTable() const { return m_foreignTable; }
        const Table* getLocalTable() const { return m_localTable; }
        const Column* getForeignColumn() const { return m_foreignColumn; }
        const Column* getLocalColumn() const { return m_localColumn; }

        QString getSForeignTable() const { return m_sforeignTable; }
        QString getSLocalTable() const { return m_slocalTable; }
        QString getSForeignColumn() const { return m_sforeignColumn; }
        QString getSLocalColumn() const { return m_slocalColumn; }

        void setForeignTable(const Table* foreignTable) {m_foreignTable = foreignTable;}
        void setForeignColumn(const Column* foreignColumn) {m_foreignColumn = foreignColumn;}
        void setLocalColumn(const Column* localColumn) {m_localColumn = localColumn;}
        void setLocalTable(const Table* localTable) {m_localTable = localTable;}

        void setSForeignTable(const QString& foreignTable) {m_sforeignTable = foreignTable;}
        void setSForeignColumn(const QString& foreignColumn) {m_sforeignColumn = foreignColumn;}
        void setSLocalColumn(const QString& localColumn) {m_slocalColumn = localColumn;}
        void setSLocalTable(const QString& localTable) {m_slocalTable = localTable;}

    private:

        const Table* m_foreignTable;
        const Table* m_localTable;
        const Column* m_foreignColumn;
        const Column* m_localColumn;

        QString m_sforeignTable;
        QString m_slocalTable;
        QString m_sforeignColumn;
        QString m_slocalColumn;

    };

public:

    ForeignKey(Version *v, const QString& uid);

    void addAssociation(ColumnAssociation* assoc)
    {
        m_associations.append(assoc);
    }

    ColumnAssociation* getAssociation(int i)
    {
        return m_associations.at(i);
    }

    const QVector<ColumnAssociation*>& getAssociations() const
    {
        return m_associations;
    }

    const QString& getOnUpdate() const
    {
        return m_onUpdate;
    }

    void setOnUpdate(const QString& onUpdate)
    {
        m_onUpdate = onUpdate;
    }

    const QString& getOnDelete() const
    {
        return m_onDelete;
    }

    void setOnDelete(const QString& onDelete)
    {
        m_onDelete = onDelete;
    }

    void removeAssociation(const QString& fcName, const QString& lcName);

    bool hasAssociation(const QString& fcName, const QString& lcName);

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    virtual QUuid getClassUid() const;

    QString getDescriptiveText() const;

    QString getLocalDescriptiveText() const;

    QString getForeignDescriptiveText() const;

    QString getForeignTableName() const;

    const Table* getForeignTable() const;
    const Table* getLocalTable() const;

    /**
     * Return a vector of the foreign columns
     */
    QVector<const Column*> foreignColumns() const;

    /**
     * Return a vector of the local columns
     */
    QVector<const Column*> localColumns() const;


    void addAutogeneratedIndex(Index* idx)
    {
        m_autoGenIndices.append(idx);
    }

    void onDelete();

    void addTempAutogenIndex(TableIndexTempStructure b)
    {
        tempautogenIndices.append(b);
    }

    void finalizeAutogeneratedIndices(Version* v);

    const QVector<Index*>& getAutoGenIndices()
    {
        return m_autoGenIndices;
    }

private:

    // the associations between the columns
    QVector<ColumnAssociation*> m_associations;

    QVector<Index*> m_autoGenIndices;

    QVector<TableIndexTempStructure> tempautogenIndices;

    QString m_onUpdate;

    QString m_onDelete;
};

#endif // FOREIGNKEY_H