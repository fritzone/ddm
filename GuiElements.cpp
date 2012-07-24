#include "GuiElements.h"
#include "MainWindow.h"
#include "Workspace.h"
#include "Solution.h"

GuiElements::GuiElements() : m_projectTreeDock(0),m_issuesTreeDock(0),
    m_genTreeDock(0), m_projectTree(0), m_issuesTree(0), m_genTree(0),
    m_contextMenuHandler(0), m_issuesContextMenuHandler(0)
{
}

void GuiElements::createGuiElements()
{
    m_contextMenuHandler = new ContextMenuHandler();
    // create the project tree dock window
    m_projectTreeDock = new QDockWidget(QObject::tr("Objects - ") + Workspace::getInstance()->currentSolution()->getName(), MainWindow::instance());
    m_projectTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_projectTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_projectTreeDock->setFloating(false);
    m_projectTreeDock->setMinimumSize(300, 340);
    m_projectTreeDock->setMaximumSize(QApplication::desktop()->screenGeometry().width() / 4, 9999);
 //    m_projectTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_projectTreeDock->resize(301,341);

    m_projectTree = new ContextMenuEnabledTreeWidget();
    m_projectTree->setAllColumnsShowFocus(true);
    m_projectTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_projectTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_projectTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_projectTree));
    m_projectTree->setColumnCount(1);
    m_projectTree->setHeaderHidden(true);
    QObject::connect(m_projectTree, SIGNAL (currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)), MainWindow::instance(), SLOT(currentProjectTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    // the issues tree dock window
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

    m_issuesTreeDock->hide();

    // create the gen tree
    m_genTreeDock = new QDockWidget(QObject::tr("Generated Items - ") + Workspace::getInstance()->currentSolution()->getName(), MainWindow::instance());
    m_genTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_genTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_genTreeDock->setFloating(false);
    m_genTreeDock->setMinimumSize(300, 340);
    m_genTreeDock->setMaximumSize(QApplication::desktop()->screenGeometry().width() / 4, 9999);
 //    m_genTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_genTreeDock->resize(301,341);

    m_genTree = new ContextMenuEnabledTreeWidget();
    m_genTree->setAllColumnsShowFocus(true);
    m_genTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_genTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_genTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_genTree));
    m_genTree->setColumnCount(1);
    m_genTree->setHeaderHidden(true);
    QObject::connect(m_genTree, SIGNAL (currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)), MainWindow::instance(), SLOT(currentProjectTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    m_genTreeDock->setWidget(m_genTree);

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
