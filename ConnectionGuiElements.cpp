#include "ConnectionGuiElements.h"
#include "MainWindow.h"
#include "core_ConnectionManager.h"
#include "core_Connection.h"
#include "ContextMenuCollection.h"

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
    m_connectionsTreeDock->setMaximumSize(500, 9999);
    m_connectionsTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_connectionsTreeDock->resize(301,341);

    m_connectionsTree = new ContextMenuEnabledTreeWidget();
    m_connectionsTree->setAllColumnsShowFocus(true);
    m_connectionsTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_connectionsTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_connectionsTree->setColumnCount(1);
    m_connectionsTree->setHeaderHidden(false);

    QTreeWidgetItem *headerItm = m_connectionsTree->headerItem();
    headerItm->setText(0, QApplication::translate("MainWindow", "Connection", 0, QApplication::UnicodeUTF8));
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
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionConnect(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onConnectConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionBrowse(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onBrowseConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionDrop(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onDropConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionDelete(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onDeleteConnection()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_ConnectionEdit(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onEditConnection()));
    QObject::connect(m_connectionsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), MainWindow::instance(), SLOT(onConnectionItemDoubleClicked(QTreeWidgetItem*,int)));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_BrowsedTableInject(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onInjectBrowsedTable()));
    QObject::connect(ContextMenuCollection::getInstance()->getAction_BrowsedTableBrowse(), SIGNAL(triggered()), MainWindow::instance(), SLOT(onBrowseBrowsedTable()));

}


ContextMenuEnabledTreeWidgetItem* ConnectionGuiElements::createConnectionTreeEntry(Connection* c)
{
    ContextMenuEnabledTreeWidgetItem* newConnectionItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, QStringList(c->getName() + "(" + c->getDb()+"@"+c->getHost() + ")")) ;
    QVariant var(c->getName());
    newConnectionItem->setData(0, Qt::UserRole, var);
    newConnectionItem->setIcon(0, c->provideIcon());
    m_connectionsTree->addTopLevelItem(newConnectionItem);
    newConnectionItem->setPopupMenu(ContextMenuCollection::getInstance()->getConnectionsPopupMenu());
    c->setLocation(newConnectionItem);
    return newConnectionItem ;
}
