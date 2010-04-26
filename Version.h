#ifndef VERSION_H
#define VERSION_H

#include <QTreeWidgetItem>

class UserDataType;
class Table;

/**
 * Basic class holding data related to versions
 */
class Version
{
public:
    Version();
    virtual ~Version();

    // the methods below will return the various tree items
    virtual QTreeWidgetItem* getDtsItem() const = 0;
    virtual QTreeWidgetItem* getTablesItem() const = 0;
    virtual QTreeWidgetItem* getViewsItem() const = 0;
    virtual QTreeWidgetItem* getVersionItem() const = 0;

    /**
     * Adds a new data type to this version
     */
    void addNewDataType(UserDataType*);

    /**
     * Returns a constant reference to the data types of the version
     */
    const QVector<UserDataType*>& getDataTypes() const
    {
        return m_dataTypes;
    }

    /**
     * Checks if this version has a data type with the specified name
     */
    bool hasDataType(const QString& name) const;

    /**
     * Returns a r/w reference to the given data type, the caller can modify it.
     */
    UserDataType* getDataType(const QString& name);

    /**
     * Returns the index of the given data type...
     */
    int getDataTypeIndex(const QString& name);

    /**
     * Adds a new table to the system
     */
    void addTable(Table*);

    /**
     * Checks if this version has the given table already. The comparison is done based on
     * the table name which is supposed to be unique.
     */
    bool hasTable(Table*);

    Table* getTable(const QString& name);

private:

    // The vector of data types. The order in it is the one the user creates the data types
    QVector<UserDataType*> m_dataTypes;

    // the tables in the system
    QVector<Table*> m_tables;
};

#endif // VERSION_H
