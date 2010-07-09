#ifndef FOREIGNKEY_H
#define FOREIGNKEY_H

#include "TreeItem.h"
#include "SerializableElement.h"
#include "Column.h"
#include "DraggableGraphicsItemForForeignKey.h"
#include "Table.h"


class ForeignKey : public TreeItem, public SerializableElement
{

public:

    /**
     * Association between a foreign table column and a local table column.
     */
    class ColumnAssociation : public TreeItem
    {
    public:

        ColumnAssociation(const Table* foreignTable, const Column* foreignColumn, const Table* localTable, const Column* localColumn):
                m_foreignTable(foreignTable), m_localTable(localTable), m_foreignColumn(foreignColumn), m_localColumn(localColumn),
                m_sforeignTable(""), m_slocalTable(""), m_sforeignColumn(""), m_slocalColumn("")
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

    ForeignKey(){}

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

    const QString& getName() const
    {
        return m_name;
    }

    void setName(const QString& name)
    {
        m_name = name;
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

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    /**                                            /\
     *                                            /  \
     * This should prepare a graphics like: __ __/name\__ ____
     *                                           \    /
     *                                            \  /
     *                                             \/
     * and the two lines after and before the space should be movable so that they move when the user moves the tables
     */
    DraggableGraphicsViewItemForForeignKey* getItem() const;

    QString getDescriptiveText() const;

    QString getLocalDescriptiveText() const;

    QString getForeignDescriptiveText() const;

    QString getForeignTable() const;

private:

    // the associations between the columns
    QVector<ColumnAssociation*> m_associations;

    QString m_name;

    QString m_onUpdate;

    QString m_onDelete;
};

#endif // FOREIGNKEY_H
