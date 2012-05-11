#include "ConnectionGuiElements.h"
#include "MainWindow.h"
#include "core_ConnectionManager.h"
#include "core_Connection.h"
#include "ContextMenuCollection.h"
#include "IconFactory.h"
#include "db_DatabaseEngine.h"
#include "strings.h"

ConnectionGuiElements::ConnectionGuiElements()
{
}

void ConnectionGuiElements::createGuiElements()
{
    // create the dock window
    m_connectionsTreeDock = new QDockWidget(QObject::tr("Connections"), MainWindow::instance());
    m_connectionsTreeDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_connectionsTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_connectionsTreeDock->setFloating(false);
    m_connectionsTreeDock->setMinimumSize(300, 340);
    m_connectionsTreeDock->setMaximumSize(QApplication::desktop()->screenGeometry().width() / 4, 9999);
    m_connectionsTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_connectionsTreeDock->resize(301,341);

    m_connectionsTree = new ContextMenuEnabledTreeWidget();
    m_connectionsTree->setAllColumnsShowFocus(true);
    m_connectionsTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_connectionsTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_connectionsTree->setColumnCount(2);
    m_connectionsTree->setHeaderHidden(false);

    QTreeWidgetItem *headerItm = m_connectionsTree->headerItem();
    headerItm->setText(0, QApplication::translate("MainWindow", "Name", 0, QApplication::UnicodeUTF8));
    headerItm->setText(1, QApplication::translate("MainWindow", "DB", 0, QApplication::UnicodeUTF8));
    m_connectionsTree->header()->setDefaultSectionSize(150);

    m_connectionsContextMenuHandler = new ContextMenuHandler();
    m_connectionsTree->setItemDelegate(new ContextMenuDelegate(m_connectionsContextMenuHandler, m_connectionsTree));
    m_connectionsTreeDock->setWidget(m_connectionsTree);
    const QVector<Connection*>& cons = ConnectionManager::instance()->connections();
    for(int i=0; i<cons.size(); i++)
    {
        Connection* c = cons.at(i);
        createConnectionTreeEntry(c);
    }
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionSqlQuery(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onSqlQueryInConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionConnect(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onConnectConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionBrowse(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onBrowseConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionDrop(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onDropConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionDelete(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onDeleteConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionEdit(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onEditConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionRecreate(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onRecreateConnection()));
    QObject::connect(m_connectionsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), MainWindow::instance(), SLOT(onConnectionItemDoubleClicked(QTreeWidgetItem*,int)));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_BrowsedTableInject(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onInjectBrowsedTable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_BrowsedTableBrowse(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onBrowseBrowsedTable()));

}


ContextMenuEnabledTreeWidgetItem* ConnectionGuiElements::createConnectionTreeEntry(Connection* c)
{
    QStringList items;
    items << c->getName() << c->getDb();
    ContextMenuEnabledTreeWidgetItem* newConnectionItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, items);
    QVariant var(c->getName());
    newConnectionItem->setData(0, Qt::UserRole, var);
    newConnectionItem->setIcon(0, IconFactory::getConnectionStateIcon(c->getState()));
    m_connectionsTree->addTopLevelItem(newConnectionItem);
    newConnectionItem->setPopupMenu(ContextMenuCollection::getInstance()->getConnectionsPopupMenu());
    c->setLocation(newConnectionItem);
    return newConnectionItem ;
}

void ConnectionGuiElements::createConnectionTreeEntryForViews(Connection *c)
{
    ContextMenuEnabledTreeWidgetItem* connViewsItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(QObject::tr("Views")));
    getConnectionsTree()->addTopLevelItem(connViewsItem);
    connViewsItem->setIcon(0, IconFactory::getViewsIcon());

    // Now do the browsing
    QStringList dbViews = c->getEngine()->getAvailableViews(c);
    for(int i=0; i<dbViews.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* newViewItem = new ContextMenuEnabledTreeWidgetItem(connViewsItem, QStringList(dbViews.at(i)));
        QVariant var(browsedViewPrefix + dbViews.at(i) + "?" + c->getName());
        newViewItem->setData(0, Qt::UserRole, var);
        //newViewItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableFromBrowsePopupMenu());
        newViewItem->setIcon(0, IconFactory::getViewIcon());
        getConnectionsTree()->addTopLevelItem(newViewItem);
    }

    // TODO: now come up with a mechanism that feeds continuously the reverse engineered tables into an object that feeds it in somewhere which
    // is used by the code completion enabled text edit when editing a table. Highly possible the destination will be the Connection object
    // since everyone has access to it.
}

void ConnectionGuiElements::createConnectionTreeEntryForProcs(Connection *c)
{
    ContextMenuEnabledTreeWidgetItem* connProcsItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(QObject::tr("Procedures")));
    getConnectionsTree()->addTopLevelItem(connProcsItem);
    connProcsItem->setIcon(0, IconFactory::getProceduresIcon());

    // Now do the browsing
    QStringList dbProcs = c->getEngine()->getAvailableStoredProcedures(c);
    for(int i=0; i<dbProcs.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* newProcItem = new ContextMenuEnabledTreeWidgetItem(connProcsItem, QStringList(dbProcs.at(i)));
        QVariant var(browsedProcPrefix + dbProcs.at(i) + "?" + c->getName());
        newProcItem->setData(0, Qt::UserRole, var);
        //newViewItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableFromBrowsePopupMenu());
        newProcItem->setIcon(0, IconFactory::getProcedureIcon());
        getConnectionsTree()->addTopLevelItem(newProcItem);
    }

    // TODO: now come up with a mechanism that feeds continuously the reverse engineered tables into an object that feeds it in somewhere which
    // is used by the code completion enabled text edit when editing a table. Highly possible the destination will be the Connection object
    // since everyone has access to it.
}


void ConnectionGuiElements::createConnectionTreeEntryForFuncs(Connection *c)
{
    ContextMenuEnabledTreeWidgetItem* connFuncsItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(QObject::tr("Functions")));
    getConnectionsTree()->addTopLevelItem(connFuncsItem);
    connFuncsItem->setIcon(0, IconFactory::getFunctionsTreeIcon());

    // Now do the browsing
    QStringList dbFuncs = c->getEngine()->getAvailableStoredFunctions(c);
    for(int i=0; i<dbFuncs.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* newFuncItem = new ContextMenuEnabledTreeWidgetItem(connFuncsItem, QStringList(dbFuncs.at(i)));
        QVariant var(browsedFuncPrefix + dbFuncs.at(i) + "?" + c->getName());
        newFuncItem->setData(0, Qt::UserRole, var);
        //newViewItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableFromBrowsePopupMenu());
        newFuncItem->setIcon(0, IconFactory::getFunctionTreeIcon());
        getConnectionsTree()->addTopLevelItem(newFuncItem);
    }

    // TODO: now come up with a mechanism that feeds continuously the reverse engineered tables into an object that feeds it in somewhere which
    // is used by the code completion enabled text edit when editing a table. Highly possible the destination will be the Connection object
    // since everyone has access to it.
}

void ConnectionGuiElements::createConnectionTreeEntryForTriggers(Connection *c)
{
    ContextMenuEnabledTreeWidgetItem* connTriggerssItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(QObject::tr("Triggers")));
    getConnectionsTree()->addTopLevelItem(connTriggerssItem);
    connTriggerssItem->setIcon(0, IconFactory::getTriggersIcon());

    // Now do the browsing
    QStringList dbTriggers = c->getEngine()->getAvailableTriggers(c);
    for(int i=0; i<dbTriggers.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* newTriggerItem = new ContextMenuEnabledTreeWidgetItem(connTriggerssItem, QStringList(dbTriggers.at(i)));
        QVariant var(browsedTriggerPrefix + dbTriggers.at(i) + "?" + c->getName());
        newTriggerItem->setData(0, Qt::UserRole, var);
        //newViewItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableFromBrowsePopupMenu());
        newTriggerItem->setIcon(0, IconFactory::getTriggerIcon());
        getConnectionsTree()->addTopLevelItem(newTriggerItem);
    }

    // TODO: now come up with a mechanism that feeds continuously the reverse engineered tables into an object that feeds it in somewhere which
    // is used by the code completion enabled text edit when editing a table. Highly possible the destination will be the Connection object
    // since everyone has access to it.
}

void ConnectionGuiElements::createConnectionTreeEntryForIndexes(Connection *c)
{
    ContextMenuEnabledTreeWidgetItem* connTriggerssItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(QObject::tr("Indexes")));
    getConnectionsTree()->addTopLevelItem(connTriggerssItem);
    connTriggerssItem->setIcon(0, IconFactory::getIndexIcon());

    // Now do the browsing
    QStringList dbIndexes = c->getEngine()->getAvailableIndexes(c);
    for(int i=0; i<dbIndexes.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* newIndexItem = new ContextMenuEnabledTreeWidgetItem(connTriggerssItem, QStringList(dbIndexes.at(i)));
        QVariant var(browsedIndexPrefix + dbIndexes.at(i) + "?" + c->getName());
        newIndexItem->setData(0, Qt::UserRole, var);
        //newViewItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableFromBrowsePopupMenu());
        newIndexItem->setIcon(0, IconFactory::getIndexIcon());
        getConnectionsTree()->addTopLevelItem(newIndexItem);
    }

    // TODO: now come up with a mechanism that feeds continuously the reverse engineered tables into an object that feeds it in somewhere which
    // is used by the code completion enabled text edit when editing a table. Highly possible the destination will be the Connection object
    // since everyone has access to it.
}


void ConnectionGuiElements::createConnectionTreeEntryForTables(Connection *c)
{
    ContextMenuEnabledTreeWidgetItem* connTablesItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(QObject::tr("Tables")));
    getConnectionsTree()->addTopLevelItem(connTablesItem);
    connTablesItem->setIcon(0, IconFactory::getTabinstIcon());

    // Now do the browsing
    QStringList dbTables = c->getEngine()->getAvailableTables(c);
    c->clearTables();
    for(int i=0; i<dbTables.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(connTablesItem, QStringList(dbTables.at(i)));
        QVariant var(browsedTablePrefix + dbTables.at(i) + "?" + c->getName());
        newTblsItem->setData(0, Qt::UserRole, var);
        newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableFromBrowsePopupMenu());
        newTblsItem->setIcon(0, IconFactory::getTabinstIcon());
        getConnectionsTree()->addTopLevelItem(newTblsItem);
        c->addTable(dbTables.at(i));
    }

    // TODO: now come up with a mechanism that feeds continuously the reverse engineered tables into an object that feeds it in somewhere which
    // is used by the code completion enabled text edit when editing a table. Highly possible the destination will be the Connection object
    // since everyone has access to it.
}

void ConnectionGuiElements::populateConnectionTreeForConnection(Connection *c)
{
    createConnectionTreeEntryForTables(c);
    createConnectionTreeEntryForViews(c);
    createConnectionTreeEntryForProcs(c);
    createConnectionTreeEntryForFuncs(c);
    createConnectionTreeEntryForTriggers(c);
    createConnectionTreeEntryForIndexes(c);
}

void ConnectionGuiElements::resetConnectionTreeForConnection(Connection *c)
{
    while(c->getLocation()->childCount()) c->getLocation()->removeChild(c->getLocation()->child(0));
    c->getLocation()->setIcon(0, IconFactory::getConnectedDatabaseIcon());
}
