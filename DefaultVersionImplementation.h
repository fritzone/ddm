#ifndef DEFAULTVERSIONIMPLEMENTATION_H
#define DEFAULTVERSIONIMPLEMENTATION_H

#include "Version.h"

class Project;

class DefaultVersionImplementation : public Version
{
public:
    DefaultVersionImplementation(QTreeWidget* tree, QTreeWidget* dttree, ContextMenuEnabledTreeWidgetItem* projectItem, Project* p);

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const = 0;

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

    virtual const QVector<Diagram*>& getDiagrams() const
    {
        return m_diagrams;
    }

    virtual Diagram* getDiagram(const QString& name);

    virtual Table* duplicateTable(Table*);

    virtual void removeForeignKeyFromDiagrams(ForeignKey*);

    virtual void setupTableParentChildRelationships();

    virtual TableInstance* instantiateTable(Table* tab, bool reason);

    virtual TableInstance* getTableInstance(const QString& );

    virtual void addTableInstance(TableInstance* inst)
    {
        m_tableInstances.append(inst);
    }

    virtual const QVector<TableInstance*> & getTableInstances() const
    {
        return m_tableInstances;
    }

    virtual UserDataType* duplicateDataType(const QString&);

    virtual void deleteDiagram(const QString&);

    virtual VersionGuiElements* getGui()
    {
        return m_guiElements;
    }

    virtual QString getVersionText()
    {
        return version;
    }
    virtual QVector<Table*> getTablesReferencingAColumnThroughForeignKeys(const Column*);

    void createTreeItems(QTreeWidget* tree = 0, QTreeWidget* dtTree = 0, ContextMenuEnabledTreeWidgetItem* projectIem = 0);

protected:
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

    VersionGuiElements* m_guiElements;

};

#endif // DEFAULTVERSIONIMPLEMENTATION_H
