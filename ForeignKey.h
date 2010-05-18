#ifndef FOREIGNKEY_H
#define FOREIGNKEY_H

#include "TreeItem.h"

class Table;
class Column;

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

private:

    // the associations between the columns
    QVector<ColumnAssociation*> m_associations;

    QString m_name;
};

#endif // FOREIGNKEY_H
