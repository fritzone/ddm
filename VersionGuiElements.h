#ifndef VERSIONGUIELEMENTS_H
#define VERSIONGUIELEMENTS_H

#include <QTreeWidget>

class Version;
class ContextMenuEnabledTreeWidgetItem;

/**
 * The scope of this class is to have a common collection point for the tree widget items of a version (DT, Sql, Table, etc...)
 * This is a GUI class, ie. has access to the QtGui elements
 */
class VersionGuiElements
{
public:

    VersionGuiElements(QTreeWidget*,QTreeWidget*,Version*);


    ContextMenuEnabledTreeWidgetItem* getTablesItem() const
    {
        return tablesItem;
    }

    ContextMenuEnabledTreeWidgetItem* getTableInstancesItem() const
    {
        return tableInstancesItem;
    }


    ContextMenuEnabledTreeWidgetItem* getFinalSqlItem() const
    {
        return finalSqlItem;
    }


    ContextMenuEnabledTreeWidgetItem* getVersionItem() const
    {
        return versionItem;
    }


    ContextMenuEnabledTreeWidgetItem* getDiagramsItem() const
    {
        return diagramsItem;
    }

    ContextMenuEnabledTreeWidgetItem* getDtsItem() const
    {
        return dtsItem;
    }

    ContextMenuEnabledTreeWidgetItem* getStringDtsItem() const
    {
        return stringsDtItem;
    }

    void createGuiElements(ContextMenuEnabledTreeWidgetItem* projectItem);

    void populateTreeItems();

private:

    // the tree item containing the "Tables"
    ContextMenuEnabledTreeWidgetItem* tablesItem;

    // the tree item containing the "Table Instances"
    ContextMenuEnabledTreeWidgetItem* tableInstancesItem;

    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* versionItem;

    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* diagramsItem;

    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* finalSqlItem;

    // the tree item containing the "DataType"
    ContextMenuEnabledTreeWidgetItem* dtsItem;

    QTreeWidget* m_tree;

    QTreeWidget* m_dtTree;

    // the tree item containing the "Strings"
    ContextMenuEnabledTreeWidgetItem* stringsDtItem;


    Version* m_version;
};

#endif // VERSIONGUIELEMENTS_H
