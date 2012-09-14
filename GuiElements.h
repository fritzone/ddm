#ifndef GUIELEMENTS_H
#define GUIELEMENTS_H

#include "ContextMenuEnabledTreeWidget.h"

#include <QtGui>

class Patch;

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

    QDockWidget* getPatchesDock() const
    {
        return m_patchesTreeDock;
    }

    ContextMenuEnabledTreeWidget* getPatchesTree() const
    {
        return m_patchesTree;
    }

    ContextMenuEnabledTreeWidgetItem* createNewPatchItem(Patch *p);
    ContextMenuEnabledTreeWidgetItem* createNewItemForPatch(Patch *p, const QString& class_uid, const QString &uid, const QString &name);
    ContextMenuEnabledTreeWidgetItem* updateItemForPatchWithState(Patch *p, const QString& class_uid, const QString &uid, const QString &name, int state);

    void removeItemForPatch(Patch *p, const QString& uid);

private:

    QDockWidget* m_projectTreeDock;
    QDockWidget* m_issuesTreeDock;
    QDockWidget* m_genTreeDock;
    QDockWidget* m_patchesTreeDock;

    ContextMenuEnabledTreeWidget* m_projectTree;
    ContextMenuEnabledTreeWidget* m_issuesTree;
    ContextMenuEnabledTreeWidget* m_genTree;
    ContextMenuEnabledTreeWidget* m_patchesTree;

    ContextMenuHandler* m_contextMenuHandler;
    ContextMenuHandler* m_issuesContextMenuHandler;

    // the vector holding the root tree items of the patches
    QMap<Patch*, ContextMenuEnabledTreeWidgetItem*> m_patchItems;
};

#endif // GUIELEMENTS_H
