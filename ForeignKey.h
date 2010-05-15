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
    private:

        Table* m_foreignTable;
        Table* m_localTable;
        Column* m_foreigColumn;
        Column* m_localColumn;
    };

public:

    ForeignKey();

private:

    // the table we are working on
    Table* m_table;

    // the associations between the columns
    QVector<ColumnAssociation*> m_associations;

    QString m_name;
};

#endif // FOREIGNKEY_H
