#ifndef _TABLE_H_
#define _TABLE_H_

#include "TreeItem.h"
#include "SerializableElement.h"
#include "SqlSourceEntity.h"

#include <QString>
#include <QVector>
#include <QGraphicsItemGroup>

class Column;
class Index;
class ForeignKey;
class AbstractStorageEngine;
class DraggableGraphicsViewItem ;
class Version;
class UserDataType;
class TableInstance;

/**
 * The table class holds a database table defined by the user. It must be derived from the TreeItem since a table can be placed in
 * the tree, so the user of it must know how to update the visual part too.
 */
class Table : virtual public TreeItem, public SerializableElement, public SqlSourceEntity
{
public:

    /**
     * Constructor, creates a new object
     */
    Table(Version* v);

    virtual ~Table() {}

    /**
     * Adds a new column to the table
     * @param column - the column to add
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
     * Returns the given Index to be found at the cth position for read/write operations
     * @param c - the index of the Index
     */
    Index* getIndex(const QString&) const;

    /**
     * Returns the column with the given name. It searches only in the current table, parent tables are not searched.
     * @param name - the name of the column we're looking for
     */
    Column* getColumn(const QString& name) const;

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

    ForeignKey* getForeignKey(const QString&) const;

    /**
     * Returns the first ForeignKey object if this table has a foreign key to the specified table
     */
    ForeignKey* getForeignKeyToTable(const QString& tableName) const;
    ForeignKey* getForeignKeyToTable(Table* tab) const;

    void setDefaultValues(QVector <QVector <QString> > &);

    const QVector <QVector <QString> > & getStartupValues() const
    {
        return m_startupValues;
    }

    /**
     * Returns the columns as a QStringList
     */
    QStringList columns() const;

    /**
     * Returns the columns and the columns of the parent tables as a QStringList
     */
    QStringList fullColumns() const;

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    const Table* getParent() const
    {
        return m_parent;
    }

    void setParent(const Table* parent)
    {
        m_parent = parent;
    }

    bool isPersistent() const
    {
        return m_persistent;
    }

    void setPersistent(bool t)
    {
        m_persistent = t;
    }

    bool isTemporary() const
    {
        return m_temporary;
    }

    void setTemporary(bool t)
    {
        m_temporary = t;
    }

    void setStorageEngine(AbstractStorageEngine* ste)
    {
        m_storageEngine = ste;
    }

    AbstractStorageEngine* getStorageEngine() const
    {
        return m_storageEngine;
    }

    DraggableGraphicsViewItem* getDiagramEntity()
    {
        prepareDiagramEntity(); // this is sort of stupid ... and might leak some memory, but otherwise it breaks the underlying QT architecture
        // TODO: remvoe the above call from everywhere it's used
        return m_diagramEntity;
    }

    void prepareDiagramEntity();

    const Table* parent() const
    {
        return m_parent;
    }

    bool hasColumn(const QString& colName) const ;

    bool parentsHaveColumn(const QString& colName) const;

    int getTotalParentColumnCount() const;

    int getColumnCount() const
    {
        return m_columns.size();
    }

    bool hasIndex(const QString&) const ;

    Index* getIndexFromParents(const QString&) const;

    ForeignKey* getForeignKeyFromParents(const QString&) const;


    void removeForeignKey(ForeignKey* toRemove);

    Version* version() const
    {
        return m_version;
    }

    void setTempTabName(const QString& s)
    {
        m_tempTabName = s;
    }

    QString getTempTabName() const
    {
        return m_tempTabName;
    }

    /**
     */
    const UserDataType* getDataTypeOfColumn(const QString& cname) const;

    /**
     * Returns the column with the given name, performs a search only in the parent tables
     */
    Column* getColumnFromParents(const QString& name) const;

    QStringList fullIndices() const;

    QStringList indices() const;

    QString getDescription() const
    {
        return m_description;
    }

    /**
     * Adds a foreign key command to this table. These commands are used in the "final SQL" rendering, in case there are two tables that each reference
     * each other through a foreign key.
     */
    void addForeignKeyCommand(const QString& com)
    {
        foreignKeyCommands << com;
    }

    void restartSqlRendering()
    {
        foreignKeyCommands.clear();
    }

    QStringList getForeignKeyCommands() const
    {
        return foreignKeyCommands;
    }

    /**
     * This returns a list of tables that are referenced by the foreign keys of this table. This is used in the instantiation process
     * so that the application know automatically to instantiate the correct tables (so that the SQL contains no errors)
     * @param exception contains the tables that shouldn't be added there
     */
    QSet<const Table*> getTablesReferencedByForeignKeys();

    virtual QStringList generateSqlSource(AbstractSqlGenerator * generator, QHash<QString,QString>);

    /**
     * returns the first available index name
     */
    QString getAvailableIndexName(const QString& prefix);

    /**
     * Creates a new index for use with the
     */
    Index* createAutoIndex(QVector<const Column*> cols);

    void addInstance(TableInstance* tinst)
    {
        tableInstances.append(tinst);
    }

    void tableInstancesAddColumn(Column* col);

    void tableInstancesRemoveColumn(Column* col);

    void tableInstancesRenameColumn(const QString& oldName, const QString& newName);


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

    // the default values as typed in in the startup values screen. Format: per row. Each QVector<QStrig> holds columnCount values
    QVector <QVector <QString> > m_startupValues;

    // tables can be built up in a OO hierarchy, each table inherits the parent table's properties (columns, indices, etc), and it can extend it
    const Table* m_parent;

    // if the table is marked as persistent the C++ (Java) code generator will generate some extra code which will load the table on instantiation
    bool m_persistent;

    // this is a temporary table. The SQL code generator for the CREATE TABLE syntax will contain TEMPORARY
    bool m_temporary;

    AbstractStorageEngine* m_storageEngine;

    // will be shown on the diagrams
    DraggableGraphicsViewItem* m_diagramEntity;

    Version* m_version;

    QString m_tempTabName;

    mutable QStringList foreignKeyCommands;

    // all the table instances that were created from this table
    QVector<TableInstance*> tableInstances;


private:

};


#endif // TABLE_H
