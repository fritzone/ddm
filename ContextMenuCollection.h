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

    virtual QAction* getAction_RenameDiagram()
    {
        return action_RenameDiagram;
    }

    virtual QAction * getAction_DuplicateTable()
    {
        return action_DuplicateTable;
    }
    virtual QAction * getAction_InstantiateTable()
    {
        return action_InstantiateTable;
    }

    virtual QAction* getAction_DuplicateDataType()
    {
        return action_DuplicateDataType;
    }

    virtual QAction* getAction_DeleteDiagram()
    {
        return action_DeleteDiagram;
    }

    virtual QMenu* getTablePopupMenu()
    {
        return m_tablePopupMenu;
    }

    virtual QMenu* getDatatypePopupMenu()
    {
        return m_datatypePopupMenu;
    }

    virtual QMenu* getTableInstancePopupMenu()
    {
        return m_tableInstancePopupMenu;
    }

    virtual QMenu* getDiagramPopupMenu()
    {
        return m_diagramPopupMenu;
    }

    virtual QAction * getAction_RemoveTable()
    {
        return action_RemoveTable;
    }

    virtual QAction * getAction_TableAddColumn()
    {
        return action_TableAddColumn;
    }

    virtual QAction * getAction_SpecializeTable()
    {
        return action_SpecializeTable;
    }

    virtual QAction * getAction_DeleteTableInstance()
    {
        return action_DeleteTableInstance;
    }

    virtual QAction * getAction_RenameTableInstance()
    {
        return action_RenameTableInstance;
    }

    virtual QAction* getAction_DeleteDataType()
    {
        return action_DeleteDataType;
    }


private:

    ContextMenuCollection();

    QMenu* m_tablePopupMenu;
    QMenu* m_tableInstancePopupMenu;
    QMenu* m_datatypePopupMenu;
    QMenu* m_diagramPopupMenu;

    QAction * action_RemoveTable;
    QAction * action_DuplicateTable;
    QAction * action_SpecializeTable;
    QAction * action_InstantiateTable;
    QAction * action_TableAddColumn;

    QAction * action_DeleteTableInstance;
    QAction * action_RenameTableInstance;

    QAction * action_DeleteDataType;
    QAction * action_DuplicateDataType;

    QAction * action_DeleteDiagram;
    QAction * action_RenameDiagram;

    static ContextMenuCollection* m_instance;
};

#endif // CONTEXTMENUHANDLER_H
