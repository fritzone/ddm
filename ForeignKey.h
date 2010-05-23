#ifndef FOREIGNKEY_H
#define FOREIGNKEY_H

#include "TreeItem.h"
#include "Column.h"

class Table;

class ForeignKey : public TreeItem
{

public:


    /**
     * Association between a foreign table column and a local table column.
     */
    class ColumnAssociation : public TreeItem
    {
    public:

        ColumnAssociation(const Table* foreignTable, const Column* foreignColumn, const Table* localTable, const Column* localColumn):
                m_foreignTable(foreignTable), m_localTable(localTable), m_foreignColumn(foreignColumn), m_localColumn(localColumn) {}

        const Table* getForeignTable() const { return m_foreignTable; }
        const Table* getLocalTable() const { return m_localTable; }
        const Column* getForeignColumn() const { return m_foreignColumn; }
        const Column* getLocalColumn() const { return m_localColumn; }

    private:

        const Table* m_foreignTable;
        const Table* m_localTable;
        const Column* m_foreignColumn;
        const Column* m_localColumn;
    };

public:

    ForeignKey(){}

    void addAssociation(ColumnAssociation* assoc)
    {
        m_associations.append(assoc);
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

    void removeAssociation(const QString& fcName, const QString& lcName)
    {
        for(int i=0; i<m_associations.size(); i++)
        {
            if(m_associations[i]->getForeignColumn()->getName() == fcName && m_associations[i]->getLocalColumn()->getName() == lcName)
            {
                m_associations.remove(i);
                return;
            }
        }
    }

private:

    // the associations between the columns
    QVector<ColumnAssociation*> m_associations;

    QString m_name;

    QString m_onUpdate;

    QString m_onDelete;
};

#endif // FOREIGNKEY_H
