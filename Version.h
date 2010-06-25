#ifndef VERSION_H
#define VERSION_H

#include "SerializableElement.h"

#include <QTreeWidgetItem>

class UserDataType;
class Table;
class Diagram;

/**
 * Basic class holding data related to versions
 */
class Version : virtual public SerializableElement
{
public:
    Version();
    virtual ~Version();

    /**
     * Return the tree item of the Data Types
     */
    virtual QTreeWidgetItem* getDtsItem() const = 0;

    /**
     * Return the tree item of the Tables
     */
    virtual QTreeWidgetItem* getTablesItem() const = 0;

    /**
     * Return the tree item of the defined queries
     */
    virtual QTreeWidgetItem* getQueriesItem() const = 0;

    /**
     * Return the tree item of the Version
     */
    virtual QTreeWidgetItem* getVersionItem() const = 0;

    /**
     * Return the tree item of the diagrams
     */
    virtual QTreeWidgetItem* getDiagramsItem() const = 0;

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
    virtual bool hasTable(Table*) = 0;

    /**
     * Return the table with the given name for r/w access
     */
    virtual Table* getTable(const QString& name) = 0;

    virtual const QVector<Table*>& getTables() const = 0;

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const = 0;

private:

};

#endif // VERSION_H
