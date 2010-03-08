#ifndef _TABLE_H_
#define _TABLE_H_

#include "TreeItem.h"

#include <QString>
#include <QVector>

class Column;

/**
 * The table class holds a database table defined by the user. It must be derived from the TreeItem since a table can be placed in
 * the tree, so the user of it must know how to update the visual part too.
 */
class Table : virtual public TreeItem
{
public:

    /**
     * Constructor, creates a new object
     */
    Table();

    Table(const Table& other) : m_name(other.m_name), m_columns(other.m_columns)
    {}



    /**
     * Adds a new column to the table
     */
    void addColumn(Column* column);

    /**
     * Moves the column down in the column list (right in the vector)
     */
    void moveColumnDown(int c);

    /**
     * Moves the column c "up" in the column list (ie. left in the vector)
     */
    void moveColumnUp(int c);

    /**
     * Returns the given column for read/write access
     */
    Column* getColumn(int c);

    /**
     * Returns the columns of this table for a read-only access
     */
    const QVector<Column*> & getColumns() const
    {
        return m_columns;
    }

    const QString& getName() const
    {
        return m_name;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }

private:
    // the name of the table
    QString m_name;

    // the columns that this table is having
    QVector<Column*> m_columns;

};

Q_DECLARE_METATYPE(Table)

#endif // TABLE_H
