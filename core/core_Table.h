#ifndef _TABLE_H_
#define _TABLE_H_

#include "TreeItem.h"
#include "SerializableElement.h"
#include "SqlSourceEntity.h"
#include "CopyableElement.h"
#include "NamedItem.h"
#include "core_ObjectWithUid.h"
#include "ObjectWithSpInstances.h"
#include "uids.h"
#include "core_CloneableElement.h"
#include "DocumentationSource.h"
#include "core_ColumnWithValue.h"

#include <QString>
#include <QVector>

class Column;
class Index;
class ForeignKey;
class Version;
class UserDataType;
class TableInstance;

/**
 * The table class holds a database table defined by the user. It must be derived from the TreeItem since a table can be placed in
 * the tree, so the user of it must know how to update the visual part too.
 */
class Table : public TreeItem,
        public SerializableElement,
        public SqlSourceEntity,
        public CopyableElement,
        public NamedItem,
        public ObjectWithUid,
        public ObjectWithSpInstances,
        public CloneableElement,
        public DocumentationSource
{
public:

    /**
     * Constructor, creates a new object
     */
    Table(Version* v, QString uid, int dummy);

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
    const QVector<ForeignKey*> & getForeignKeys() const
    {
        return m_foreignKeys;
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

    const QVector <QVector <QString> > & getDefaultValues() const
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

    void setParent(Table* parent);

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

    Version* getVersion() const
    {
        return m_version;
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
        m_foreignKeyCommands << com;
    }

    void restartSqlRendering()
    {
        m_foreignKeyCommands.clear();
    }

    QStringList getForeignKeyCommands() const
    {
        return m_foreignKeyCommands;
    }

    /**
     * This returns a list of tables that are referenced by the foreign keys of this table. This is used in the instantiation process
     * so that the application know automatically to instantiate the correct tables (so that the SQL contains no errors)
     * @param exception contains the tables that shouldn't be added there
     */
    QSet<const Table*> getTablesReferencedByForeignKeys();

    virtual QStringList generateSqlSource(AbstractSqlGenerator * generator, QHash<QString,QString>, const Connection*);

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
        m_tableInstances.append(tinst);
    }

    const QVector<TableInstance*>& getTableInstances() const
    {
        return m_tableInstances;
    }

    void tableInstancesAddColumn(Column* col);

    void tableInstancesRemoveColumn(Column* col);

    void tableInstancesRenameColumn(const QString& oldName, const QString& newName);

    virtual void copy();

    /**
     * This should return the first foreign key the column is participating in, otherwise 0
     */
    QVector<ForeignKey*> columnParticipatesInForeignKey(const Column* col);

    QString generateUniqueColumnName(const QString& in);

    void addSpecializedTable(const Table *childTable);

    bool hasSpecializedTables() const;

    void removeSpecializedTable(Table*);

    Table* getParent() const;

    virtual QUuid getClassUid() const;

    void setParentUid(const QString&);

    Column* getColumnByUid(const QString&) const;

    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);

    QString getTempTabName() const
    {
        return m_tempTabName;
    }
    void setTempTabName(const QString& s)
    {
        m_tempTabName = s;
    }

    QString getParentUid() const
    {
        return m_parentUid;
    }

    virtual void updateGui();

    int getIndexOfColumn(const Column*);
    int getIndexOfColumn(const QString&);

    virtual QString getSqlHash() const { return "N/A"; }

    QSet<Column*> primaryKeyColumns() const;

    /**
     * Returns a vector of vector of startup values filtered to the given columns/value
     * @brief getValues
     * @return
     */
    QVector <QVector<ColumnWithValue*> > getValues(QVector<ColumnWithValue*> vals) const;

    /**
     * Returns all the startup values in the specified format
     * @brief getFullValues
     * @return
     */
    QVector <QVector<ColumnWithValue*> > getFullValues() const;

private:

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
    Table* m_parent;

    Version* m_version;

    mutable QStringList m_foreignKeyCommands;

    // all the table instances that were created from this table
    QVector<TableInstance*> m_tableInstances;

    // all the specialized tables that were created from this
    QVector<const Table*> m_children;

    QString m_parentUid;

    QString m_tempTabName;
private:

};


#endif // TABLE_H
