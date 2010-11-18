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
class VersionGuiElements;
class Column;

/**
 * Basic class holding data related to versions
 */
class Version : virtual public SerializableElement
{
public:

    Version() {};
    virtual ~Version() {};

    /**
     * Returns a constant reference to the data types of the version
     */
    virtual const QVector<UserDataType*>& getDataTypes() const = 0;

    /**
     * Returns the tables stored in this version
     */
    virtual const QVector<Table*>& getTables() const = 0;

    /**
     * Return the table instances
     */
    virtual const QVector<TableInstance*> & getTableInstances() const = 0;

    /**
     * Return the diagrams
     */
    virtual const QVector<Diagram*> & getDiagrams() const = 0;

    /**
     * Adds a new data type to this version
     */
    virtual void addNewDataType(UserDataType*) = 0;

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
     * Deletes a table from the system. Also resolves the FK dependencies:
     * 1. deletes the table and the foreign table referencing this table
     * 2. Deletes the FK reference from the foreign table
     */
    virtual void deleteTable(Table*) = 0;

    /**
     * Deletes the given table instance, and also all the other table instances
     * that were instantiated due to foreign keys referencing this table instance
     * when it was instantiated
     */
    virtual void deleteTableInstance(TableInstance*) = 0;

    /**
     * Duplicates the table
     */
    virtual Table* duplicateTable(Table*) = 0;

    /**
     * Sets up a parent child relationship. Used in the factory methods of creating a version
     */
    virtual void setupTableParentChildRelationships() = 0;

    /**
     * Sets up the foreign key relationships. Used in the factory methods of creating a version
     */
    virtual void setupForeignKeyRelationshipsForATable(Table* tab) = 0;

    /**
     * Instantiates a table from the given table template
     */
    virtual TableInstance* instantiateTable(Table* tab, bool reason) = 0;

    /**
     * Adds a new table instance to this version
     */
    virtual void addTableInstance(TableInstance* inst) = 0;

    /**
     * Returns a table instance
     */
    virtual TableInstance* getTableInstance(const QString& ) = 0;

    /**
     * Purges the table instances that were sentenced in a "delete table instance" procedure
     */
    virtual void purgeSentencedTableInstances() = 0;

    /**
     * Deletes the given data type
     */
    virtual void deleteDataType(const QString&) = 0;

    /**
     * Duplicates the given data type
     */
    virtual UserDataType* duplicateDataType(const QString&) = 0;

    /**
     * Deletes a diagram from this version
     */
    virtual void deleteDiagram(const QString&) = 0;

    /**
     * Returns the GUI elements (the Version tree widget item, and the otehrs) of this version
     */
    virtual VersionGuiElements* getGui() = 0;

    /**
     * Returns the version as a human readable string
     */
    virtual QString getVersionText() = 0;

    /**
     * Return a vector of tables that are referencing the given column through a foreign key
     */
    virtual QVector<Table*> getTablesReferencingAColumnThroughForeignKeys(const Column*) = 0;

    virtual QList<QString> getSqlScript() = 0;

    /**
     * Provides a data type for the given SQL type. Firstly checks if there are data types corresponding to the
     * given name/size pair and and if found one, returns it. If did not find any, creates a new one, adds to the
     * vector of data types and returns it.
     */
    virtual UserDataType* provideDatatypeForSqlType(const QString& sql, const QString& nullable, const QString& defaultValue) = 0;

private:

};

#endif // VERSION_H
