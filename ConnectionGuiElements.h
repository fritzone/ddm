#ifndef CONNECTIONGUIELEMENTS_H
#define CONNECTIONGUIELEMENTS_H

#include "ContextMenuEnabledTreeWidget.h"

#include <QtGui>

class Connection;

class ConnectionGuiElements
{
public:
    ConnectionGuiElements();
    void createGuiElements();

    QDockWidget* getConnectionsTreeDock() const
    {
        return m_connectionsTreeDock;
    }

    ContextMenuEnabledTreeWidget* getConnectionsTree() const
    {
        return m_connectionsTree;
    }

    ContextMenuEnabledTreeWidgetItem* createConnectionTreeEntry(Connection* c);


private:

    QDockWidget* m_connectionsTreeDock;

    ContextMenuEnabledTreeWidget* m_connectionsTree;

    ContextMenuHandler* m_connectionsContextMenuHandler;
};

#endif // CONNECTIONGUIELEMENTS_H
