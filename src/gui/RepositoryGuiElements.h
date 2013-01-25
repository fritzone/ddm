#ifndef REPOSITORYGUIELEMENTS_H
#define REPOSITORYGUIELEMENTS_H

#include "ContextMenuEnabledTreeWidget.h"

#include <QtGui>

class DatabaseEngine;
class Repository;
class Entity;

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

    ContextMenuEnabledTreeWidgetItem* createDatabaseeTreeEntry(DatabaseEngine* dbEngine);
    ContextMenuEnabledTreeWidgetItem* createEntityTreeEntry(Entity* entity);


private:
    QDockWidget* m_repositoryTreeDock;

    ContextMenuEnabledTreeWidget* m_repositoryTree;

    ContextMenuHandler* m_connectionsContextMenuHandler;

    ContextMenuEnabledTreeWidgetItem* m_databasesTreeEntry;
    ContextMenuEnabledTreeWidgetItem* m_rolesTreeEntry;
    ContextMenuEnabledTreeWidgetItem* m_entitiesTreeEntry;
    Repository* m_repo;
};

#endif // REPOSITORYGUIELEMENTS_H
