#ifndef _TABLE_H_
#define _TABLE_H_

#include "TreeItem.h"
#include "SerializableElement.h"

#include <QString>
#include <QVector>

class Column;
class Index;
class ForeignKey;

/**
 * The table class holds a database table defined by the user. It must be derived from the TreeItem since a table can be placed in
 * the tree, so the user of it must know how to update the visual part too.
 */
class Table : virtual public TreeItem, public SerializableElement
{
public:

    /**
     * Constructor, creates a new object
     */
    Table();

    /**
     * copy constructor
     */
    Table(const Table& other) : m_name(other.m_name), m_columns(other.m_columns), m_indices(other.m_indices)
    {}

    /**
     * Adds a new column to the table
     */
    void addColumn(Column* column);

    /**
     * Add an index to the table
     */
    void addIndex(Index* index);

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
     * @param c - is the index of the column
     */
    Column* getColumn(int c);

    /**
     * Returns the given Index to be found at the cth position for read/write operations
     * @param c - the index of the Index
     */
    Index* getIndex(int c);

    /**
     * Returns the column with the given name for read only access
     */
    const Column* getColumn(const QString& name);

    /**
     * Returns the columns of this table for a read-only access
     */
    const QVector<Column*> & getColumns() const
    {
        return m_columns;
    }

    /**
     * Returns the indices of this table for a read-only access
     */
    const QVector<Index*> & getIndices() const
    {
        return m_indices;
    }


    /**
     * Returns the foreign keys of this table for a read-only access
     */
    const QVector<ForeignKey*> & getFks() const
    {
        return m_foreignKeys;
    }

    const QString& getName() const
    {
        return m_name;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }

    void setDescription(const QString& desc)
    {
        m_description = desc;
    }

    void removeIndex(Index* toRemove);

    void removeColumn(Column* toRemove);

    /**
     * Returns true if this column is used in an index. In this case the column cannot be deleted.
     */
    Index* isColumnUsedInIndex(const Column*);

    void addForeignKey(ForeignKey* fk)
    {
        m_foreignKeys.append(fk);
    }

    ForeignKey* getForeignKey(int i);



    /**
     * Returns the columns as a QStringList
     */
    QStringList columns() const;

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

private:
    // the name of the table
    QString m_name;

    // describes the table
    QString m_description;

    // the columns that this table is having
    QVector<Column*> m_columns;

    // the indices of the table
    QVector<Index*> m_indices;

    // the vector will hold the foreign keys
    QVector<ForeignKey*> m_foreignKeys;

};

Q_DECLARE_METATYPE(Table)

#endif // TABLE_H
