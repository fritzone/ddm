#ifndef VERSION_H
#define VERSION_H

#include "SerializableElement.h"

#include <QTreeWidgetItem>

class UserDataType;
class Table;
class ForeignKey;
class Diagram;
class ContextMenuEnabledTreeWidgetItem;
class TableInstance;
class DatabaseEngine;

/**
 * Basic class holding data related to versions
 */
class Version : virtual public SerializableElement
{
public:
    Version() {};
    virtual ~Version() {};

    /**
     * Return the tree item of the Data Types
     */
    virtual ContextMenuEnabledTreeWidgetItem* getDtsItem() const = 0;

    /**
     * Return the tree item of the Tables
     */
    virtual ContextMenuEnabledTreeWidgetItem* getTablesItem() const = 0;

    /**
     * Return the tree item of the final SQL script
     */
    virtual ContextMenuEnabledTreeWidgetItem* getFinalSqlItem() const = 0;

    /**
     * Return the tree item of the defined queries
     */
    virtual ContextMenuEnabledTreeWidgetItem* getTableInstancesItem() const = 0;

    /**
     * Return the tree item of the Version
     */
    virtual ContextMenuEnabledTreeWidgetItem* getVersionItem() const = 0;

    /**
     * Return the tree item of the diagrams
     */
    virtual ContextMenuEnabledTreeWidgetItem* getDiagramsItem() const = 0;

    /**
     * Adds a new data type to this version
     */
    virtual void addNewDataType(UserDataType*) = 0;

    /**
     * Returns a constant reference to the data types of the version
     */
    virtual const QVector<UserDataType*>& getDataTypes() const = 0;

    /**
     * Checks if this version has a data type with the specified name
     */
    virtual bool hasDataType(const QString& name) const = 0;

    /**
     * Returns a r/w reference to the given data type, the caller can modify it.
     */
    virtual UserDataType* getDataType(const QString& name) = 0;

    /**
     * Returns the index of the given data type...
     */
    virtual int getDataTypeIndex(const QString& name) = 0;

    /**
     * Adds a new table to the system
     */
    virtual void addTable(Table*) = 0;

    /**
     * Add a diagram to the system
     */
    virtual void addDiagram(Diagram*) = 0;


    /**
     * Checks if this version has the given table already. The comparison is done based on
     * the table name which is supposed to be unique.
     */
    virtual bool hasTable(const QString&) = 0;

    /**
     * Checks if this version has the given table already. The comparison is done based on
     * the table name which is supposed to be unique.
     */
    virtual bool hasTable(Table*) = 0;

    /**
     * Return the table with the given name for r/w access
     */
    virtual Table* getTable(const QString& name) = 0;

    /**
     * Returns the tables stored in this version
     */
    virtual const QVector<Table*>& getTables() const = 0;

    /**
     * Serializes this version
     */
    virtual void serialize(QDomDocument &doc, QDomElement &parent) const = 0;

    /**
     * Returns the diagram with the given name
     */
    virtual Diagram* getDiagram(const QString& name) = 0;

    /**
     * Removes the foreign key from teh version
     */
    virtual void removeForeignKeyFromDiagrams(ForeignKey*) = 0;

    /**
     * Deletes a table from the system. Also resolves the FK dependencies: 1. deletes the table and the foreign table referencing this table 2. Deletes the FK reference from the foreign table
     */
    virtual void deleteTable(Table*) = 0;

    virtual void deleteTableInstance(TableInstance*) = 0;

    /**
     * Duplicates the table
     */
    virtual Table* duplicateTable(Table*) = 0;

    /**
     * Sets up a parent child relationship
     */
    virtual void setupTableParentChildRelationships() = 0;

    /**
     * Instantiates a table from the given table template
     */
    virtual TableInstance* instantiateTable(Table* tab, bool reason) = 0;

    /**
     * Returns true if this version is in a project which has OOP features enabled
     */
    virtual bool oop() = 0;

    /**
     * Adds a new table instance to this version
     */
    virtual void addTableInstance(TableInstance* inst) = 0;

    /**
     * Returns a table instance
     */
    virtual TableInstance* getTableInstance(const QString& ) = 0;

    virtual const QVector<TableInstance*> & getTableInstances() = 0;

    virtual void purgeSentencedTableInstances() = 0;

    virtual DatabaseEngine* getDatabaseEngine() = 0;

    // don't know if this is a good design or not for the popups ....

    virtual QMenu* getTablePopupMenu() = 0;
    virtual QAction * getAction_RemoveTable() = 0;
    virtual QAction * getAction_TableAddColumn() = 0;
    virtual QAction * getAction_SpecializeTable() = 0;
    virtual QAction * getAction_DuplicateTable() = 0;
    virtual QAction * getAction_InstantiateTable() = 0;

    virtual QMenu* getTableInstancePopupMenu() = 0;
    virtual QAction * getAction_DeleteTableInstance() = 0;

private:

};

#endif // VERSION_H
