#ifndef CONNECTIONGUIELEMENTS_H
#define CONNECTIONGUIELEMENTS_H

#include "ContextMenuEnabledTreeWidget.h"

#include <QObject>
#include <QtGui>

class Connection;

class ConnectionGuiElements : public QObject
{
    Q_OBJECT
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
    void resetConnectionTreeForConnection(Connection *c);
    void populateConnectionTreeForConnection(Connection *c);

    void createConnectionTreeEntryForViews(Connection *c);
    void createConnectionTreeEntryForTables(Connection *c);
    void createConnectionTreeEntryForProcs(Connection *c);
    void createConnectionTreeEntryForFuncs(Connection *c);
    void createConnectionTreeEntryForTriggers(Connection *c);
    void createConnectionTreeEntryForIndexes(Connection *c);

private slots:
    void connectionItemActivated(QTreeWidgetItem*,int);
    void newTable();
    void newConnection();

private:

    QDockWidget* m_connectionsTreeDock;

    ContextMenuEnabledTreeWidget* m_connectionsTree;

    ContextMenuHandler* m_connectionsContextMenuHandler;

    ContextMenuEnabledTreeWidgetItem* m_mysqlConnections;
    ContextMenuEnabledTreeWidgetItem* m_sqliteConnections;

    QToolBar *bar;
    QAction* action_connectionNewTable;
    QAction* action_newConnection;
    Connection* m_lastConnection;
};

#endif // CONNECTIONGUIELEMENTS_H
