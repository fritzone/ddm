#ifndef MAJORVERSION_H
#define MAJORVERSION_H

#include "Version.h"
#include "ContextMenuEnabledTreeWidget.h"

#include <QString>
#include <QTreeWidgetItem>

class Project;

/**
 * Class representing a major version of the database. Major versions are always baselined versions, ie. they contain only
 * the elements that (technically) do not contain the changes vectors. Minor versions contain changes vectors, and a
 * reference to the Major version with the required data types. Changes vectors are three vectors for each type (UDT, Index, Table ,...)
 * one of them is holding the value that was deleted, one of them is holding the value that  was changed and one of them is
 * holding the value that was updated. When the minor version is baselined (for ex: 1.75 -> 2.0) into a major one a new MajorVersion object is created
 * and the final list of types is written into it. When a new (minor) version is released (for ex: 1.0 -> 1.1) a new MinorVersion object is
 * created with the changes that were performed on the major version since its loading ... This means that we need to track the changes of
 * the major version starting from the point it was "loaded".
 */
class MajorVersion : public Version
{
public:
    /**
     * Constructor: Takes in the project Item and the current version
     */
    MajorVersion(QTreeWidget* tree, QTreeWidget* dttree, ContextMenuEnabledTreeWidgetItem* projectItem, int ver, Project*);

    /**
     * This constructos is used when deserializing. In this case the tree and projectItem are set later
     */
    MajorVersion(QString verAsString, Project*);

    virtual ContextMenuEnabledTreeWidgetItem* getDtsItem() const
    {
        return dtsItem;
    }

    virtual ContextMenuEnabledTreeWidgetItem* getFinalSqlItem() const
    {
        return finalSqlItem;
    }

    virtual ContextMenuEnabledTreeWidgetItem* getTablesItem() const
    {
        return tablesItem;
    }

    virtual ContextMenuEnabledTreeWidgetItem* getTableInstancesItem() const
    {
        return tableInstancesItem;
    }

    virtual ContextMenuEnabledTreeWidgetItem* getVersionItem() const
    {
        return versionItem;
    }

    virtual ContextMenuEnabledTreeWidgetItem* getDiagramsItem() const
    {
        return diagramsItem;
    }

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    virtual void addNewDataType(UserDataType*);

    virtual void addDiagram(Diagram*);

    virtual const QVector<UserDataType*>& getDataTypes() const;

    virtual bool hasDataType(const QString& name) const;

    virtual UserDataType* getDataType(const QString& name);

    virtual int getDataTypeIndex(const QString& name);

    virtual void addTable(Table*);

    virtual bool hasTable(Table*);
    virtual bool hasTable(const QString&);

    virtual void deleteTable(Table*);
    virtual void deleteTableInstance(TableInstance*);
    virtual void purgeSentencedTableInstances();

    virtual void deleteDataType(const QString&);

    virtual Table* getTable(const QString& name);

    virtual const QVector<Table*>& getTables() const;

    void createTreeItems(QTreeWidget* tree = 0, QTreeWidget* dtTree = 0, ContextMenuEnabledTreeWidgetItem* projectIem = 0);

    /**
     * Populates the tree items of this major version with all the data it contains, such as UserDataTypes, Tables, etc...
     */
    void populateTreeItems();

    virtual Diagram* getDiagram(const QString& name);

    virtual Table* duplicateTable(Table*);

    virtual void removeForeignKeyFromDiagrams(ForeignKey*);

    virtual void setupTableParentChildRelationships();

    virtual TableInstance* instantiateTable(Table* tab, bool reason);

    virtual TableInstance* getTableInstance(const QString& );

    virtual bool oop();

    virtual DatabaseEngine* getDatabaseEngine();

    virtual void addTableInstance(TableInstance* inst)
    {
        m_tableInstances.append(inst);
    }

    virtual const QVector<TableInstance*> & getTableInstances()
    {
        return m_tableInstances;
    }

    virtual UserDataType* duplicateDataType(const QString&);

    virtual void deleteDiagram(const QString&);

private:

    // the tree item containing the "Tables"
    ContextMenuEnabledTreeWidgetItem* tablesItem;

    // the tree item containing the "Views"
    ContextMenuEnabledTreeWidgetItem* tableInstancesItem;

    // the tree item containing the "DataType"
    ContextMenuEnabledTreeWidgetItem* dtsItem;

    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* versionItem;

    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* diagramsItem;
    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* finalSqlItem;

    // the version as a string representation. Major versions are always of form X.0
    QString version;

    // The vector of data types. The order in it is the one the user creates the data types
    QVector<UserDataType*> m_dataTypes;

    // the tables in the system
    QVector<Table*> m_tables;

    QVector<Diagram*> m_diagrams;

    QVector<TableInstance*> m_tableInstances;

    // the project tree
    QTreeWidget* m_tree;
    // the  data types tree
    QTreeWidget* m_dtTree;

    ContextMenuEnabledTreeWidgetItem* m_projectItem;

    Project* m_project;
};

#endif // MAJORVERSION_H
