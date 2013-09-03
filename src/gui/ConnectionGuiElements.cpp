#include "ConnectionGuiElements.h"
#include "MainWindow.h"
#include "ConnectionManager.h"
#include "Connection.h"
#include "ContextMenuCollection.h"
#include "IconFactory.h"
#include "db_DatabaseEngine.h"
#include "strings.h"

ConnectionGuiElements::ConnectionGuiElements() :
    m_connectionsTreeDock(0),
    m_connectionsTree(0),
    m_connectionsContextMenuHandler(0),
    m_mysqlConnections(0),
    m_sqliteConnections(0),
    m_cubridConnections(0),
    bar(0),
    action_connectionNewTable(0),
    action_newConnection(0),
    m_lastConnection(0)

{
}

void ConnectionGuiElements::createGuiElements()
{
    QStringList supportedDbs = QSqlDatabase::drivers();

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
    m_connectionsTree->setColumnCount(1);
    m_connectionsTree->setHeaderHidden(false);

    QTreeWidgetItem *headerItm = m_connectionsTree->headerItem();
    headerItm->setText(0, QApplication::translate("MainWindow", "Name", 0, QApplication::UnicodeUTF8));
    m_connectionsTree->header()->setDefaultSectionSize(250);


    // create the MySql connection tree item
    if(supportedDbs.contains(strQMySql))
    {
        QStringList lstMySqlStr;
        lstMySqlStr << strCamelMySql;
        m_mysqlConnections = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, lstMySqlStr);
        m_mysqlConnections->setIcon(0, IconFactory::getMySqlIcon());
        m_connectionsTree->addTopLevelItem(m_mysqlConnections);
        m_mysqlConnections->setData(0, Qt::UserRole, QVariant(connectionGroupPrefix + strMySql));
    }

    // create the Sqlite connection tree item
    if(supportedDbs.contains(strQSqlite))
    {
        QStringList lstSqliteStr;
        lstSqliteStr<< strCamelSqlite;
        m_sqliteConnections = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, lstSqliteStr);
        m_sqliteConnections->setIcon(0, IconFactory::getSqliteIcon());
        m_connectionsTree->addTopLevelItem(m_sqliteConnections);
        m_sqliteConnections->setData(0, Qt::UserRole, QVariant(connectionGroupPrefix + strSqlite));
    }

    // create the CUBRID connection tree item
    if(supportedDbs.contains(strQCUBRID))
    {
        QStringList lstCUBRIDStr;
        lstCUBRIDStr << strCUBRID;
        m_cubridConnections = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, lstCUBRIDStr);
        m_cubridConnections->setIcon(0, IconFactory::getCUBRIDIcon());
        m_connectionsTree->addTopLevelItem(m_cubridConnections);
        m_cubridConnections->setData(0, Qt::UserRole, QVariant(connectionGroupPrefix + strCUBRID));
    }

    // context handler
    m_connectionsContextMenuHandler = new ContextMenuHandler();
    m_connectionsTree->setItemDelegate(new ContextMenuDelegate(m_connectionsContextMenuHandler, m_connectionsTree));

    // and finally all the connections
    const QVector<Connection*>& cons = ConnectionManager::instance()->connections();
    for(int i=0; i<cons.size(); i++)
    {
        Connection* c = cons.at(i);
        createConnectionTreeEntry(c);
    }

    QObject::connect(m_connectionsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), MainWindow::instance(), SLOT(onConnectionItemDoubleClicked(QTreeWidgetItem*,int)));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_BrowsedTableInject(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onInjectBrowsedTable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_BrowsedTableBrowse(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onBrowseBrowsedTable()));

    QObject::connect(m_connectionsTree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(connectionItemActivated(QTreeWidgetItem*,int)));

    QMainWindow *window = new QMainWindow(0);

    bar = new QToolBar(window);
    bar->setObjectName(QString::fromUtf8("connectionToolbar"));
    bar->setEnabled(true);
    bar->setAcceptDrops(true);
    bar->setMovable(false);
    bar->setIconSize(QSize(16, 16));
    bar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    bar->setFloatable(false);

    window->setParent(m_connectionsTreeDock);
    window->setCentralWidget(m_connectionsTree);
    m_connectionsTreeDock->setWidget(window);

    action_connectionNewTable = new QAction(tr("New Table"), 0);
    action_connectionNewTable->setIcon(IconFactory::getTabinstIcon());
    QObject::connect(action_connectionNewTable, SIGNAL(triggered()), this, SLOT(newTable()));
    action_connectionNewTable->setDisabled(true);

    action_newConnection = new QAction(tr("New Connection"), 0);
    action_newConnection->setIcon(IconFactory::getDatabaseIcon());
    QObject::connect(action_newConnection, SIGNAL(triggered()), this, SLOT(newConnection()));

    bar->addAction(action_newConnection);
    bar->addAction(action_connectionNewTable);

    window->addToolBar(bar);
}


void ConnectionGuiElements::connectionItemActivated(QTreeWidgetItem* item,int)
{
    QString d = item->data(0, Qt::UserRole).toString();

    // filter out to which connection the entry belongs
    Connection* c = ConnectionManager::instance()->getConnection(d);
    if(c)
    {
        m_lastConnection = c;
    }

    if(m_lastConnection)
    {
        action_connectionNewTable->setDisabled(false);
        action_connectionNewTable->setToolTip(tr("Create new table in: <b>") +
                                              m_lastConnection->getName());
    }

    //qDebug() << d;
}

void ConnectionGuiElements::newConnection()
{
    MainWindow::instance()->onNewConnection();
}

void ConnectionGuiElements::newTable()
{
    MainWindow::instance()->createTableInConnection(m_lastConnection, false);
}

ContextMenuEnabledTreeWidgetItem* ConnectionGuiElements::createConnectionTreeEntry(Connection* c)
{
    QStringList items;
    items << c->getName();
    ContextMenuEnabledTreeWidgetItem* parent = 0;
    QString dbType = c->getDbType().toUpper();
    if(dbType == strMySql.toUpper())
    {
        parent = m_mysqlConnections;
    }
    if(dbType == strSqlite.toUpper())
    {
        parent = m_sqliteConnections;
    }
    if(dbType == strCUBRID.toUpper())
    {
        parent = m_cubridConnections;
    }

    ContextMenuEnabledTreeWidgetItem* newConnectionItem = new ContextMenuEnabledTreeWidgetItem(parent, items);
    QVariant var(c->getName());
    newConnectionItem->setData(0, Qt::UserRole, var);
    newConnectionItem->setIcon(0, IconFactory::getConnectionStateIcon(c->getState()));
    m_connectionsTree->addTopLevelItem(newConnectionItem);
    newConnectionItem->setPopupMenu(ContextMenuCollection::getInstance()->getConnectionsPopupMenu(c->getEngine()->getName()));
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
    if(c->getDbType().toUpper() != strSqlite.toUpper())
    {
        createConnectionTreeEntryForProcs(c);
        createConnectionTreeEntryForFuncs(c);
    }
    createConnectionTreeEntryForTriggers(c);
    createConnectionTreeEntryForIndexes(c);
}

void ConnectionGuiElements::resetConnectionTreeForConnection(Connection *c)
{
    while(c->getLocation()->childCount() > 0) c->getLocation()->removeChild(c->getLocation()->child(0));
    c->getLocation()->setIcon(0, IconFactory::getConnectedDatabaseIcon());
}
