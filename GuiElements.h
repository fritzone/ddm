#ifndef GUIELEMENTS_H
#define GUIELEMENTS_H

#include "ContextMenuEnabledTreeWidget.h"

#include <QtGui>

class GuiElements
{
public:
    GuiElements();
    void createGuiElements();
    void freeGuiElements();

    QDockWidget* getProjectDock() const
    {
        return m_projectTreeDock;
    }

    ContextMenuEnabledTreeWidget* getProjectTree() const
    {
        return m_projectTree;
    }

    QDockWidget* getDatatypesDock() const
    {
        return m_datatypesTreeDock;
    }

    ContextMenuEnabledTreeWidget* getDataTypesTree() const
    {
        return m_datatypesTree;
    }

    QDockWidget* getIssuesDock() const
    {
        return m_issuesTreeDock;
    }

    ContextMenuEnabledTreeWidget* getIssuesTree() const
    {
        return m_issuesTree;
    }

private:

    QDockWidget* m_projectTreeDock;
    QDockWidget* m_datatypesTreeDock;
    QDockWidget* m_issuesTreeDock;

    ContextMenuEnabledTreeWidget* m_projectTree;
    ContextMenuEnabledTreeWidget* m_datatypesTree;
    ContextMenuEnabledTreeWidget* m_issuesTree;

    ContextMenuHandler* m_contextMenuHandler;
    ContextMenuHandler* m_issuesContextMenuHandler;

};

#endif // GUIELEMENTS_H
