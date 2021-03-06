#ifndef CONTEXTMENUHANDLER_H
#define CONTEXTMENUHANDLER_H

#include <QAction>
#include <QMenu>

class DBMenu;

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
    QMenu* getProceduresPopupMenu() { return m_proceduresPopupMenu; }
    QMenu* getFunctionsPopupMenu() { return m_functionsPopupMenu; }
    QMenu* getTriggersPopupMenu() { return m_triggersPopupMenu; }
    QMenu* getColumnPopupMenu() { return m_columnPopupMenu; }
    QMenu* getDatatypesPopupMenu() { return m_datatypesPopupMenu; }
    QMenu* getIssuePopupMenu() { return m_issuePopupMenu; }
    QMenu* getIssuesOfATablePopupMenuPopupMenu() { return m_issuesOfATablePopupMenu; }
    QMenu* getCreateTableInstancesPopupMenu() { return m_createTableInstancesPopup; }
    QMenu* getCreateNewViewPopupMenu() {return m_createNewViewPopupMenu; }
    QMenu* getCreateNewProcedurePopupMenu() {return m_createNewProcedurePopupMenu; }
    QMenu* getCreateNewFunctionPopupMenu() {return m_createNewFunctionPopupMenu; }
    QMenu* getDeployPopupMenu() {return m_deployPopupMenu; }
    QMenu* getViewsPopupMenu() { return m_viewsPopupMenu; }
    QMenu* getTableFromBrowsePopupMenu() { return m_browsedTablePopupMenu; }
    QMenu* getViewPopupMenu() { return m_viewPopupMenu; }
    QMenu* getProcedurePopupMenu() { return m_procedurePopupMenu; }
    QMenu* getMajorVersionPopupMenu() { return m_majorVersionPopupMenu; }
    QMenu* getUnlockTablePopupMenu() { return m_unlockTablePopupMenu; }
    QMenu* getRelockTablePopupMenu() { return m_relockTablePopupMenu; }
    QMenu* getUnlockTableInstancePopupMenu() { return m_unlockTableInstancePopupMenu; }
    QMenu* getRelockTableInstancePopupMenu() { return m_relockTableInstancePopupMenu; }
    QMenu* getUnlockViewPopupMenu() { return m_unlockViewPopupMenu; }
    QMenu* getRelockViewPopupMenu() { return m_relockViewPopupMenu; }
    QMenu* getUnlockDiagramPopupMenu() { return m_unlockDiagramPopupMenu; }
    QMenu* getRelockDiagramPopupMenu() { return m_relockDiagramPopupMenu; }
    QMenu* getUnlockDataTypePopupMenu() { return m_unlockDTPopupMenu; }
    QMenu* getRelockDataTypePopupMenu() { return m_relockDTPopupMenu; }
    QMenu* getUnlockTriggerPopupMenu() { return m_unlockTriggerPopupMenu; }
    QMenu* getRelockTriggerPopupMenu() { return m_relockTriggerPopupMenu; }
    QMenu* getUnlockProcedurePopupMenu() { return m_unlockProcedurePopupMenu; }
    QMenu* getRelockProcedurePopupMenu() { return m_relockProcedurePopupMenu; }
    QMenu* getUnlockFunctionPopupMenu() { return m_unlockFunctionPopupMenu; }
    QMenu* getRelockFunctionPopupMenu() { return m_relockFunctionPopupMenu; }
    QMenu* getFunctionPopupMenu() { return m_functionPopupMenu; }
    QMenu* getTriggerPopupMenu() { return m_triggerPopupMenu; }
    QMenu* getSuspendPatchPopupMenu() { return m_finalizePatchMenu; }
    QMenu* getResumePatchPopupMenu() { return m_resumePatchMenu; }
    QMenu* getUndeletePopupMenu() { return m_undeletePopupMenu; }
    QMenu* getReleasedVersionPopupMenu() { return m_releasedVersionPopupMenu; }
    QMenu* getFinalisedVersionPopupMenu() { return m_finalisedVersionPopupMenu; }
    QMenu* getRepositoryDatabasesPopupMenu() { return m_repositoryDatabasesPopupMenu; }

    QMenu* getConnectionsPopupMenu(const QString& dbName);

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
    QAction* getAction_CreateGuidedProcedure() { return action_createGuidedProcedure; }
    QAction* getAction_CreateGuidedFunction() { return action_createGuidedFunction; }
    QAction* getAction_CreateSqlProcedure() { return action_createSqlProcedure; }
    QAction* getAction_CreateSqlFunction() { return action_createSqlFunction; }
    QAction* getAction_BrowsedTableInject() {return action_browsedTableInjectIntoSolution; }
    QAction* getAction_BrowsedTableView() {return action_browsedTableView; }
    QAction* getAction_BrowsedTableBrowse() {return action_browsedTableBrowse; }
    QAction* getAction_DeleteView() {return action_deleteView; }
    QAction* getAction_DeleteProcedure() {return action_deleteProcedure; }
    QAction* getAction_DeleteFunction() {return action_deleteFunction; }
    QAction* getAction_DeleteTrigger() {return action_deleteTrigger; }
    QAction* getAction_ReleaseMajorVersion() {return action_releaseMajorVersion; }
    QAction* getAction_unlock() {return action_unlock; }
    QAction* getAction_relock() {return action_relock; }
    QAction* getAction_FinalizePatch() { return action_finalizePatch; }
    QAction* getAction_ResumePatch() { return action_resumePatch; }
    QAction* getAction_RenamePatch() { return action_renamePatch; }
    QAction* getAction_RelocateElementToAnotherPatch() { return action_relocateElementToAnotherPatch; }
    QAction* getAction_Undelete() { return action_undelete; }
    QAction* getAction_AddProcedure() { return action_addProcedure; }
    QAction* getAction_AddFunction() { return action_addFunction; }
    QAction* getAction_AddTrigger() { return action_addTrigger; }
    QAction* getAction_AddView() { return action_addView; }
    QAction* getAction_AddViewWithSql() { return action_addViewWithSql; }
    QAction* getAction_InitiatePatch() { return action_initiatePatch; }
    QAction* getAction_CompareTable() { return action_compareTable; }
    QAction* getAction_DeployVersion() { return action_deployVersion; }

    QMenu* getUnLockMenuForClassUid(const QString& uid);
    QMenu* getMenuForClassUid(const QString& uid);
    QMenu* getReLockMenuForClassUid(const QString& uid);

    /**
     * @brief getMenu generate a connection menu from the given DBMenu object
     * @return
     */
    QMenu* getDBMenu(const DBMenu*);

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
    QMenu* m_createTableInstancesPopup;         // popup menu for the table instances selection
    QMenu* m_createNewViewPopupMenu;            // popup for the create new view command.
    QMenu* m_createNewProcedurePopupMenu;       // popup for the create new procedure command.
    QMenu* m_createNewFunctionPopupMenu;        // popup for the create new function command.
    QMenu* m_deployPopupMenu;                   // popup menu for the deployment, will contain the list of connections
    QMenu* m_viewsPopupMenu;                    // popup menu for the views tree element
    QMenu* m_viewPopupMenu;                     // popup menu for one view tree element
    QMenu* m_browsedTablePopupMenu;             // popup menu for a table which was retrieved from the browse connection
    QMenu* m_procedurePopupMenu;                // popup menu for one procedure tree element
    QMenu* m_majorVersionPopupMenu;             // popup menu for one major version tree element
    QMenu* m_unlockTablePopupMenu;              // popup menu for unlocking one major version table element which was locked after being released
    QMenu* m_relockTablePopupMenu;              // popup menu for re-locking one major version table element which was unlocked by the user
    QMenu* m_unlockTableInstancePopupMenu;      // popup menu for unlocking one major version table instance element which was locked after being released
    QMenu* m_relockTableInstancePopupMenu;      // popup menu for re-locking one major version table instance element which was unlocked by the user
    QMenu* m_unlockDiagramPopupMenu;            // popup menu for unlocking one major version diagram element which was locked after being released
    QMenu* m_relockDiagramPopupMenu;            // popup menu for re-locking one major version diagram element which was unlocked by the user
    QMenu* m_unlockTriggerPopupMenu;            // popup menu for unlocking one major version trigger element which was locked after being released
    QMenu* m_relockTriggerPopupMenu;            // popup menu for re-locking one major version trigger element which was unlocked by the user
    QMenu* m_unlockProcedurePopupMenu;          // popup menu for unlocking one major version procedure element which was locked after being released
    QMenu* m_relockProcedurePopupMenu;          // popup menu for re-locking one major version procedure element which was unlocked by the user
    QMenu* m_unlockFunctionPopupMenu;           // popup menu for unlocking one major version function element which was locked after being released
    QMenu* m_relockFunctionPopupMenu;           // popup menu for re-locking one major version function element which was unlocked by the user
    QMenu* m_unlockDTPopupMenu;                 // popup menu for unlocking one major version data type element which was locked after being released
    QMenu* m_relockDTPopupMenu;                 // popup menu for re-locking one major version data type element which was unlocked by the user
    QMenu* m_unlockViewPopupMenu;               // popup menu for unlocking one major version view element which was locked after being released
    QMenu* m_relockViewPopupMenu;               // popup menu for re-locking one major version view element which was unlocked by the user
    QMenu* m_undeletePopupMenu;                 // popup menu for undeleting something from a patch which was deleted by the user
    QMenu* m_functionPopupMenu;                 // popup menu for a function
    QMenu* m_triggerPopupMenu;                  // popup menu for a trigger
    QMenu* m_triggersPopupMenu;                 // popup menu for the triggers
    QMenu* m_finalizePatchMenu;                 // popup menu for an unsuspended patch
    QMenu* m_resumePatchMenu;                   // popup menu for a suspended patch
    QMenu* m_proceduresPopupMenu;               // popup menu for the procedures tree entry
    QMenu* m_functionsPopupMenu;                // popup menu for the procedures tree entry
    QMenu* m_releasedVersionPopupMenu;          // popup menu for the version which was just released. Contains a "Initiate patch" and a "Deploy" entry
    QMenu* m_finalisedVersionPopupMenu;         // popup menu for the version which was finalise. Contains a "Deploy" entry
    QMenu* m_repositoryDatabasesPopupMenu;      // Popup menu for  the repository, "Databases"

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

    // the procedures popup menu
    QAction* action_createGuidedProcedure;
    QAction* action_createSqlProcedure;

    // the functions popup menu
    QAction* action_createGuidedFunction;
    QAction* action_createSqlFunction;

    // a browsed table popup menu
    QAction* action_browsedTableInjectIntoSolution;
    QAction* action_browsedTableCopyToConnection;
    QAction* action_browsedTableBrowse; // see the data of the table
    QAction* action_browsedTableView;   // see the details of the table

    // the view menu's actions
    QAction* action_deleteView;

    // the procedure menu's actions
    QAction* action_deleteProcedure;

    // the major version popup menu
    QAction* action_releaseMajorVersion;

    // actions for a locked/unlocked element
    QAction* action_unlock;
    QAction* action_relock;

    // actions for the functions popup menu
    QAction* action_deleteFunction;

    // actions for the trigger popup menu
    QAction* action_deleteTrigger;

    // actions for the patch, when it is not suspended
    QAction* action_finalizePatch;
    QAction* action_renamePatch;

    // action for the patch when it is suspended
    QAction* action_resumePatch;

    // action for moving an element from a patch to a different patch
    QAction* action_relocateElementToAnotherPatch;

    // a more generic undelete action
    QAction* action_undelete;

    // for the procedures menu
    QAction* action_addProcedure;

    // for the functions menu
    QAction* action_addFunction;

    // for the triggers menu
    QAction* action_addTrigger;

    // for the views menu
    QAction* action_addView;
    QAction* action_addViewWithSql;

    // for the released version
    QAction* action_initiatePatch;
    QAction* action_deployVersion;

    // comparison for tables
    QAction* action_compareTable;

    // the instance of the popup menu collection
    static ContextMenuCollection* m_instance;
};

#endif // CONTEXTMENUHANDLER_H
