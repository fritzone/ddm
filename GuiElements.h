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

    QDockWidget* getGenDock() const
    {
        return m_genTreeDock;
    }

    QDockWidget* getProjectDock() const
    {
        return m_projectTreeDock;
    }

    ContextMenuEnabledTreeWidget* getProjectTree() const
    {
        return m_projectTree;
    }

    QDockWidget* getIssuesDock() const
    {
        return m_issuesTreeDock;
    }

    ContextMenuEnabledTreeWidget* getIssuesTree() const
    {
        return m_issuesTree;
    }

    ContextMenuEnabledTreeWidget* getGenTree() const
    {
        return m_genTree;
    }

private:

    QDockWidget* m_projectTreeDock;
    QDockWidget* m_issuesTreeDock;
    QDockWidget* m_genTreeDock;

    ContextMenuEnabledTreeWidget* m_projectTree;
    ContextMenuEnabledTreeWidget* m_issuesTree;
    ContextMenuEnabledTreeWidget* m_genTree;

    ContextMenuHandler* m_contextMenuHandler;
    ContextMenuHandler* m_issuesContextMenuHandler;

};

#endif // GUIELEMENTS_H
