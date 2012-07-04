#ifndef CONTEXTMENUHANDLER_H
#define CONTEXTMENUHANDLER_H

#include <QAction>
#include <QMenu>

/**
 * Singleton class holding the context menus
 */
class ContextMenuCollection
{
public:

    static ContextMenuCollection* getInstance()
    {
        if(m_instance == 0)
        {
            m_instance = new ContextMenuCollection();
        }
        return m_instance;
    }

    /*
     * Popup menus
     */

    QMenu* getTablePopupMenu() { return m_tablePopupMenu; }
    QMenu* getTablesPopupMenu() { return m_tablesPopupMenu; }
    QMenu* getDatatypePopupMenu() { return m_datatypePopupMenu; }
    QMenu* getTableInstancePopupMenu() { return m_tableInstancePopupMenu; }
    QMenu* getDiagramPopupMenu() { return m_diagramPopupMenu; }
    QMenu* getTableInstancesPopupMenu() { return m_tableInstancesPopupMenu; }
    QMenu* getDiagramsPopupMenu() { return m_diagramsPopupMenu; }
    QMenu* getColumnPopupMenu() { return m_columnPopupMenu; }
    QMenu* getDatatypesPopupMenu() { return m_datatypesPopupMenu; }
    QMenu* getIssuePopupMenu() { return m_issuePopupMenu; }
    QMenu* getIssuesOfATablePopupMenuPopupMenu() { return m_issuesOfATablePopupMenu; }
    QMenu* getCreateTableInstancesPopupMenu() { return m_createTableInstancesPopup; }
    QMenu* getCreateNewViewPopupMenu() {return m_createNewViewPopupMenu; }
    QMenu* getConnectionsPopupMenu() { return m_connectionPopupMenu; }
    QMenu* getDeployPopupMenu() {return m_deployPopupMenu; }
    QMenu* getViewsPopupMenu() { return m_viewsPopupMenu; }
    QMenu* getTableFromBrowsePopupMenu() { return m_browsedTablePopupMenu; }
    QMenu* getViewPopupMenu() { return m_viewPopupMenu; }
    QMenu* getProcedurePopupMenu() { return m_procedurePopupMenu; }
    QMenu* getMajorVersionPopupMenu() { return m_majorVersionPopupMenu; }

    /*
     * Actions
     */

    QAction* getAction_RenameDiagram() { return action_RenameDiagram; }
    QAction* getAction_DuplicateTable() { return action_DuplicateTable; }
    QAction* getAction_PasteTable() { return action_PasteTable; }
    QAction* getAction_AddTable() { return action_AddTable; }
    QAction* getAction_AddTableInstance() { return action_AddTableInstance; }
    QAction* getAction_InstantiateTable() { return action_InstantiateTable; }
    QAction* getAction_CopyTable() { return action_CopyTable; }
    QAction* getAction_DuplicateDataType() { return action_DuplicateDataType; }
    QAction* getAction_DeleteDiagram() { return action_DeleteDiagram; }
    QAction* getAction_RemoveTable() { return action_RemoveTable; }
    QAction* getAction_TableAddColumn() { return action_TableAddColumn; }
    QAction* getAction_SpecializeTable() { return action_SpecializeTable; }
    QAction* getAction_DeleteTableInstance() { return action_DeleteTableInstance; }
    QAction* getAction_RenameTableInstance() { return action_RenameTableInstance; }
    QAction* getAction_DeleteDataType() { return action_DeleteDataType; }
    QAction* getAction_AddDiagram() { return action_AddDiagram; }
    QAction* getAction_CopyColumn() { return action_CopyColumn; }
    QAction* getAction_PasteColumn() { return action_PasteColumn; }
    QAction* getAction_AddString() { return action_addString; }
    QAction* getAction_AddNumeric() { return action_addNumeric; }
    QAction* getAction_AddDateType() { return action_addDateType; }
    QAction* getAction_AddBool() { return action_addBool; }
    QAction* getAction_AddMisc() { return action_addMisc; }
    QAction* getAction_AddBlob() { return action_addBlob; }
    QAction* getAction_AddSpatial() { return action_addSpatial; }
    QAction* getAction_GotoIssueLocation() { return action_gotoIssueLocation; }
    QAction* getAction_IgnoreIssue() { return action_ignoreIssue; }
    QAction* getAction_IgnoreIssuesFromThisTable() { return action_ignoreIssuesFromThisTable; }
    QAction* getAction_CreateViewUsingQueryBuilder() { return action_createViewUsingQueryBuilder; }
    QAction* getAction_CreateViewUsingSql() { return action_createViewUsingSql; }
    QAction* getAction_ConnectionSqlQuery() {return action_connectionSqlQuery; }
    QAction* getAction_ConnectionConnect() {return action_connectionConnect; }
    QAction* getAction_ConnectionDelete() {return action_connectionDelete; }
    QAction* getAction_ConnectionEdit() {return action_connectionEdit; }
    QAction* getAction_ConnectionRecreate() {return action_connectionRecreate; }
    QAction* getAction_ConnectionBrowse() {return action_connectionBrowse; }
    QAction* getAction_ConnectionDrop() {return action_connectionDrop; }
    QAction* getAction_BrowsedTableInject() {return action_browsedTableInjectIntoSolution; }
    QAction* getAction_BrowsedTableView() {return action_browsedTableView; }
    QAction* getAction_BrowsedTableBrowse() {return action_browsedTableBrowse; }
    QAction* getAction_DeleteView() {return action_deleteView; }
    QAction* getAction_DeleteProcedure() {return action_deleteProcedure; }
    QAction* getAction_ReleaseMajorVersion() {return action_releaseMajorVersion; }

private:

    ContextMenuCollection();

    QMenu* m_tablePopupMenu;                    // popup menu for a table
    QMenu* m_tableInstancePopupMenu;            // popup menu for a table instance
    QMenu* m_datatypePopupMenu;                 // popup menu for a datatype
    QMenu* m_diagramPopupMenu;                  // popup menu for a diagram
    QMenu* m_tablesPopupMenu;                   // popup menu for the list of tables
    QMenu* m_tableInstancesPopupMenu;           // popup menu for the list of table instance
    QMenu* m_diagramsPopupMenu;                 // popup menu for the diagram list
    QMenu* m_columnPopupMenu;                   // popup menu for a column in a table
    QMenu* m_datatypesPopupMenu;                // popup menu for the datatypes
    QMenu* m_issuePopupMenu;                    // popup menu for an issue
    QMenu* m_issuesOfATablePopupMenu;           // popup menu for issues of a table
    QMenu* m_connectionPopupMenu;               // popup menu for a connection
    QMenu* m_createTableInstancesPopup;         // popup menu for the table instances selection
    QMenu* m_createNewViewPopupMenu;            // popup for the create new view command.
    QMenu* m_deployPopupMenu;                   // popup menu for the deployment, will contain the list of connections
    QMenu* m_viewsPopupMenu;                    // popup menu for the views tree element
    QMenu* m_viewPopupMenu;                     // popup menu for one view tree element
    QMenu* m_browsedTablePopupMenu;             // popup menu for a table which was retrieved from the browse connection
    QMenu* m_procedurePopupMenu;                // popup menu for one procedure tree element
    QMenu* m_majorVersionPopupMenu;             // popup menu for one major version tree element

    // the actions in the table popup menu
    QAction* action_RemoveTable;
    QAction* action_DuplicateTable;
    QAction* action_SpecializeTable;
    QAction* action_InstantiateTable;
    QAction* action_TableAddColumn;
    QAction* action_CopyTable;

    // the actions in the table instance popup menu
    QAction* action_DeleteTableInstance;
    QAction* action_RenameTableInstance;

    // the actions in the datatype menu
    QAction* action_DeleteDataType;
    QAction* action_DuplicateDataType;

    // the actions in the diagram menu
    QAction* action_DeleteDiagram;
    QAction* action_RenameDiagram;

    // the actions in the Tables popup menu
    QAction* action_AddTable;
    QAction* action_PasteTable;

    // the actions for the Table instances popup menu
    QAction* action_AddTableInstance;

    // the actions for the Table instances popup menu
    QAction* action_AddDiagram;

    // actions from the columns list (NewTableForm)
    QAction* action_CopyColumn;
    QAction* action_PasteColumn;

    QAction* action_addString;
    QAction* action_addNumeric;
    QAction* action_addDateType;
    QAction* action_addBool;
    QAction* action_addMisc;
    QAction* action_addBlob;
    QAction* action_addSpatial;

    // the issues popup menu's actions
    QAction* action_gotoIssueLocation;
    QAction* action_ignoreIssue;

    // issues from a table actions
    QAction* action_ignoreIssuesFromThisTable;

    // the view popup menu
    QAction* action_createViewUsingQueryBuilder;
    QAction* action_createViewUsingSql;

    // a connections popup menu
    QAction* action_connectionConnect;
    QAction* action_connectionDelete;
    QAction* action_connectionEdit;
    QAction* action_connectionBrowse;
    QAction* action_connectionDrop;
    QAction* action_connectionRecreate;
    QAction* action_connectionSqlQuery;

    // a browsed table popup menu
    QAction* action_browsedTableInjectIntoSolution;
    QAction* action_browsedTableCopyToConnection;
    QAction* action_browsedTableBrowse; // see the data of the table
    QAction* action_browsedTableView;   // see the details of the table

    QAction* action_deleteView;

    QAction* action_deleteProcedure;

    QAction* action_releaseMajorVersion;

    // the instance of the popup menu collection
    static ContextMenuCollection* m_instance;
};

#endif // CONTEXTMENUHANDLER_H
