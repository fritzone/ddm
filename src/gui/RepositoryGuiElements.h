#ifndef REPOSITORYGUIELEMENTS_H
#define REPOSITORYGUIELEMENTS_H

#include "ContextMenuEnabledTreeWidget.h"

#include <QtGui>

class RepositoryGuiElements
{
public:
    RepositoryGuiElements();
    void createGuiElements();
    QDockWidget* getTreeDock() const
    {
        return m_repositoryTreeDock;
    }

    ContextMenuEnabledTreeWidget* getTree() const
    {
        return m_repositoryTree;
    }

private:
    QDockWidget* m_repositoryTreeDock;

    ContextMenuEnabledTreeWidget* m_repositoryTree;

    ContextMenuHandler* m_connectionsContextMenuHandler;
};

#endif // REPOSITORYGUIELEMENTS_H
