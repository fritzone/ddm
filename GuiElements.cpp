#include "GuiElements.h"
#include "MainWindow.h"
#include "Workspace.h"
#include "Solution.h"

GuiElements::GuiElements() : m_projectTreeDock(0), m_datatypesTreeDock(0), m_projectTree(0), m_datatypesTree(0), m_contextMenuHandler(0)
{
}

void GuiElements::createGuiElements()
{
    m_contextMenuHandler = new ContextMenuHandler();
    // create the dock window
    m_projectTreeDock = new QDockWidget(QObject::tr("Solution - ") + Workspace::getInstance()->currentSolution()->name(), MainWindow::instance());
    m_projectTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_projectTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_projectTreeDock->setFloating(false);
    m_projectTreeDock->setMinimumSize(300, 340);
    m_projectTreeDock->setMaximumSize(500, 9999);
    m_projectTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_projectTreeDock->resize(301,341);

    m_datatypesTreeDock = new QDockWidget(QObject::tr("DataTypes") , MainWindow::instance());
    m_datatypesTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_datatypesTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_datatypesTreeDock->setFloating(false);
    m_datatypesTreeDock->setMinimumSize(300, 340);
    m_datatypesTreeDock->setMaximumSize(500, 9999);

    m_projectTree = new ContextMenuEnabledTreeWidget();
    m_projectTree->setAllColumnsShowFocus(true);
    m_projectTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_projectTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_projectTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_projectTree));
    m_projectTree->setColumnCount(1);
    m_projectTree->setHeaderHidden(true);
    QObject::connect(m_projectTree, SIGNAL (currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)), MainWindow::instance(), SLOT(currentProjectTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    m_datatypesTree = new ContextMenuEnabledTreeWidget();
    m_datatypesTree ->setColumnCount(2);
    QTreeWidgetItem *hdr = m_datatypesTree->headerItem();
    hdr->setText(0, QObject::tr("Type"));
    hdr->setText(1, QObject::tr("SQL"));
    m_datatypesTree->header()->setDefaultSectionSize(200);
    m_datatypesTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_datatypesTree));
    m_datatypesTree ->setHeaderHidden(false);
    QObject::connect(m_datatypesTree, SIGNAL(itemSelectionChanged()), MainWindow::instance(), SLOT(onDTTreeClicked()));
    QObject::connect(m_datatypesTree, SIGNAL (itemClicked ( QTreeWidgetItem * , int ) ), MainWindow::instance(), SLOT(dtTreeItemClicked(QTreeWidgetItem*,int)));

    m_issuesTreeDock = new QDockWidget(QObject::tr("Issues"), MainWindow::instance());
    m_issuesTreeDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    m_issuesTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_issuesTreeDock->setFloating(false);
    m_issuesTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));

    m_issuesTree = new ContextMenuEnabledTreeWidget();
    m_issuesTree->setObjectName(QString::fromUtf8("m_issuesTree"));
    m_issuesTree->setAnimated(false);
    m_issuesTree->setExpandsOnDoubleClick(true);
    m_issuesTree->header()->setDefaultSectionSize(150);
    QTreeWidgetItem *headerItem = m_issuesTree->headerItem();
    headerItem->setText(0, QApplication::translate("MainWindow", "", 0, QApplication::UnicodeUTF8));
    headerItem->setText(1, QApplication::translate("MainWindow", "Type", 0, QApplication::UnicodeUTF8));
    headerItem->setText(2, QApplication::translate("MainWindow", "Origin", 0, QApplication::UnicodeUTF8));
    headerItem->setText(3, QApplication::translate("MainWindow", "Description", 0, QApplication::UnicodeUTF8));
    m_issuesContextMenuHandler = new ContextMenuHandler();
    m_issuesTree->setItemDelegate(new ContextMenuDelegate(m_issuesContextMenuHandler, m_issuesTree));

    m_issuesTreeDock->setWidget(m_issuesTree);
    m_projectTreeDock->setWidget(m_projectTree);
    m_datatypesTreeDock->setWidget(m_datatypesTree);

    m_issuesTreeDock->hide();

}

void GuiElements::freeGuiElements()
{

    if(m_projectTree)
    {
        delete m_projectTree;
        m_projectTree = 0;
    }

    if(m_projectTreeDock)
    {
        delete m_projectTreeDock;
        m_projectTreeDock = 0;
    }

    if(m_datatypesTree)
    {
        delete m_datatypesTree;
        m_datatypesTree = 0;
    }

    if(m_datatypesTreeDock)
    {
        delete m_datatypesTreeDock;
        m_datatypesTreeDock = 0;
    }

    if(m_issuesTree)
    {
        delete m_issuesTree;
        m_issuesTree = 0;
    }

    if(m_issuesTreeDock)
    {
        delete m_issuesTreeDock;
        m_issuesTreeDock = 0;
    }
}
