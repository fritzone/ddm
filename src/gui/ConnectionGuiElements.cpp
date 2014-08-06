#include "ConnectionGuiElements.h"
#include "MainWindow.h"
#include "ConnectionManager.h"
#include "Connection.h"
#include "ContextMenuCollection.h"
#include "IconFactory.h"
#include "db_DatabaseEngine.h"
#include "strings.h"

#include <QApplication>
#include <QDockWidget>
#include <QDesktopWidget>
#include <QHeaderView>

ConnectionGuiElements::ConnectionGuiElements() :
    m_connectionsTreeDock(0),
    m_connectionsTree(0),
    m_connectionsContextMenuHandler(0),
    bar(0),
    action_connectionNewTable(0),
    action_newConnection(0),
    m_lastConnection(0)

{
}

void ConnectionGuiElements:: createConnectionForDbEngine(const QString& dbEngineNameCC, const QIcon& icon, const QString& text)
{
    QString dbEngineName = dbEngineNameCC.toUpper();
    QStringList lstEntry;
    lstEntry << text;
    m_connectionTreeEntries[dbEngineName] = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, lstEntry);
    m_connectionTreeEntries[dbEngineName]->setIcon(0, icon);
    m_connectionsTree->addTopLevelItem(m_connectionTreeEntries[dbEngineName]);
    m_connectionTreeEntries[dbEngineName]->setData(0, Qt::UserRole, QVariant(connectionGroupPrefix + dbEngineName));
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
    headerItm->setText(0, QObject::tr("Name"));
    m_connectionsTree->header()->setDefaultSectionSize(250);


    // create the MySql connection tree item
    if(supportedDbs.contains(strQMySql))
    {
        createConnectionForDbEngine(strMySql, IconFactory::getMySqlIcon(), strCamelMySql);
    }

    // create the Sqlite connection tree item
    if(supportedDbs.contains(strQSqlite))
    {
        createConnectionForDbEngine(strSqlite, IconFactory::getSqliteIcon(), strCamelSqlite);
    }

    // create the CUBRID connection tree item
    if(supportedDbs.contains(strQCUBRID))
    {
        createConnectionForDbEngine(strCUBRID, IconFactory::getCUBRIDIcon(), strCUBRID);
    }

    // create the Postgres connection tree item
    if(supportedDbs.contains(strQPostgres))
    {
        createConnectionForDbEngine(strPostgres, IconFactory::getPostgresIcon(), strPostgres);
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

    Connection* c = ConnectionManager::instance()->getConnection(d);
    if(c)
    {
        m_lastConnection = c;
    }
    else
    {
        m_lastConnection = 0;
        action_connectionNewTable->setDisabled(true);
        action_connectionNewTable->setToolTip(tr("Create new table"));
    }

    if(m_lastConnection)
    {
        action_connectionNewTable->setDisabled(false);
        action_connectionNewTable->setToolTip(tr("Create new table in: <b>") +
                                              m_lastConnection->getName());
    }
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
    ContextMenuEnabledTreeWidgetItem* parent = m_connectionTreeEntries[c->getDbType().toUpper()];
    ContextMenuEnabledTreeWidgetItem* newConnectionItem = new ContextMenuEnabledTreeWidgetItem(parent, items);
    QVariant var(c->getName());
    newConnectionItem->setData(0, Qt::UserRole, var);
    newConnectionItem->setIcon(0, IconFactory::getConnectionStateIcon(c->getState()));
    m_connectionsTree->addTopLevelItem(newConnectionItem);
    DatabaseEngine* e = c->getEngine();
    if(e)
    {
        newConnectionItem->setPopupMenu(ContextMenuCollection::getInstance()->getConnectionsPopupMenu(e->getName()));
        c->setLocation(newConnectionItem);
    }

    return newConnectionItem ;
}

void ConnectionGuiElements::createGenericConnectionTreeEntry(Connection* c,
                                                             const QString& parentName,
                                                             const QIcon &parentIcon,
                                                             const QStringList& childItems,
                                                             const QIcon& childIcons,
                                                             const QString& prefix)
{
    ContextMenuEnabledTreeWidgetItem* genItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(parentName));
    getConnectionsTree()->addTopLevelItem(genItem);
    genItem->setIcon(0, parentIcon);

    for(int i=0; i<childItems.size(); i++)
    {
        ContextMenuEnabledTreeWidgetItem* newChildItem = new ContextMenuEnabledTreeWidgetItem(genItem, QStringList(childItems.at(i)));
        QVariant var(prefix + childItems.at(i) + "?" + c->getName());
        newChildItem->setData(0, Qt::UserRole, var);
        newChildItem->setIcon(0, childIcons);
        getConnectionsTree()->addTopLevelItem(newChildItem);
    }
}

void ConnectionGuiElements::createConnectionTreeEntryForViews(Connection *c)
{
    createGenericConnectionTreeEntry(c, QObject::tr("Views"),
                                     IconFactory::getViewsIcon(),
                                     c->getEngine()->getAvailableViews(c),
                                     IconFactory::getViewIcon(),
                                     browsedViewPrefix);
}

void ConnectionGuiElements::createConnectionTreeEntryForProcs(Connection *c)
{
    createGenericConnectionTreeEntry(c, QObject::tr("Procedures"),
                                     IconFactory::getProceduresIcon(),
                                     c->getEngine()->getAvailableStoredProcedures(c),
                                     IconFactory::getProcedureIcon(),
                                     browsedProcPrefix);
}


void ConnectionGuiElements::createConnectionTreeEntryForFuncs(Connection *c)
{
    createGenericConnectionTreeEntry(c, QObject::tr("Functions"),
                                     IconFactory::getFunctionsTreeIcon(),
                                     c->getEngine()->getAvailableStoredFunctions(c),
                                     IconFactory::getFunctionTreeIcon(),
                                     browsedFuncPrefix);
}

void ConnectionGuiElements::createConnectionTreeEntryForTriggers(Connection *c)
{
    createGenericConnectionTreeEntry(c, QObject::tr("Triggers"),
                                     IconFactory::getTriggersIcon(),
                                     c->getEngine()->getAvailableTriggers(c),
                                     IconFactory::getTriggerIcon(),
                                     browsedTriggerPrefix);
}

void ConnectionGuiElements::createConnectionTreeEntryForIndexes(Connection *c)
{
    createGenericConnectionTreeEntry(c, QObject::tr("Indexes"),
                                     IconFactory::getIndexIcon(),
                                     c->getEngine()->getAvailableIndexes(c),
                                     IconFactory::getIndexIcon(),
                                     browsedIndexPrefix);
}


void ConnectionGuiElements::createConnectionTreeEntryForTables(Connection *c)
{
    ContextMenuEnabledTreeWidgetItem* connTablesItem = new ContextMenuEnabledTreeWidgetItem(c->getLocation(), QStringList(QObject::tr("Tables")));
    getConnectionsTree()->addTopLevelItem(connTablesItem);
    connTablesItem->setIcon(0, IconFactory::getTabinstIcon());

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
}

void ConnectionGuiElements::populateConnectionTreeForConnection(Connection *c)
{
    createConnectionTreeEntryForTables(c);
    createConnectionTreeEntryForViews(c);
    if(c->getEngine()->storedMethodSupport())
    {
        createConnectionTreeEntryForProcs(c);
        createConnectionTreeEntryForFuncs(c);
    }
    createConnectionTreeEntryForTriggers(c);
    createConnectionTreeEntryForIndexes(c);
}

void ConnectionGuiElements::resetConnectionTreeForConnection(Connection *c)
{
    while(c->getLocation()->childCount() > 0)
    {
        c->getLocation()->removeChild(c->getLocation()->child(0));
    }
    c->getLocation()->setIcon(0, IconFactory::getConnectedDatabaseIcon());
}
