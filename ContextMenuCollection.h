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

    // the instance of the popup menu collection
    static ContextMenuCollection* m_instance;
};

#endif // CONTEXTMENUHANDLER_H
