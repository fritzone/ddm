#include "GuiElements.h"
#include "MainWindow.h"
#include "Workspace.h"
#include "Solution.h"
#include "core_Patch.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"

GuiElements::GuiElements() : m_projectTreeDock(0),m_issuesTreeDock(0),
    m_genTreeDock(0), m_patchesTreeDock(0),
    m_projectTree(0), m_issuesTree(0), m_genTree(0), m_patchesTree(0),
    m_contextMenuHandler(0), m_issuesContextMenuHandler(0),
    m_patchItems()
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

    // the patches dockable window
    m_patchesTreeDock = new QDockWidget(QObject::tr("Patches"), MainWindow::instance());
    m_patchesTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_patchesTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_patchesTreeDock->setFloating(false);
    m_patchesTreeDock->setMinimumSize(300, 340);
    m_patchesTreeDock->setMaximumSize(QApplication::desktop()->screenGeometry().width() / 4, 9999);
    m_patchesTreeDock->resize(301,341);

    m_patchesTree = new ContextMenuEnabledTreeWidget();
    m_patchesTree->setAllColumnsShowFocus(true);
    m_patchesTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_patchesTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_patchesTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_patchesTree));
    m_patchesTree->setColumnCount(1);
    m_patchesTree->setHeaderHidden(true);
    QObject::connect(m_patchesTree, SIGNAL (currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)), MainWindow::instance(), SLOT(currentPatchTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    m_patchesTreeDock->setWidget(m_patchesTree);
    m_patchesTreeDock->hide();

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

    if(m_patchesTreeDock)
    {
        delete m_patchesTreeDock;
        m_patchesTreeDock = 0;
    }
}

ContextMenuEnabledTreeWidgetItem* GuiElements::createNewPatchItem(Patch* p)
{
    // make the tables sub item coming from the version
    ContextMenuEnabledTreeWidgetItem* patchItem = new ContextMenuEnabledTreeWidgetItem(0, QStringList(p->getName()));
    patchItem->setIcon(0, IconFactory::getPatchIcon());
    patchItem->setPopupMenu(ContextMenuCollection::getInstance()->getSuspendPatchPopupMenu());
    m_patchesTree->addTopLevelItem(patchItem);
    //QUuid tablesUid = QUuid::createUuid();
    //patchItem->setData(0, Qt::UserRole, QVariant(tablesUid));
    //UidWarehouse::instance().addElement(tablesUid, m_version);
    m_patchItems[p] = patchItem;
}

ContextMenuEnabledTreeWidgetItem* GuiElements::createNewItemForPatch(Patch *p, const QString& class_uid, const QString& uid, const QString& name)
{
    if(!m_patchItems.contains(p))
    {
        createNewPatchItem(p);
    }
    ContextMenuEnabledTreeWidgetItem* newItem = new ContextMenuEnabledTreeWidgetItem(m_patchItems[p], QStringList(name));
    newItem->setIcon(0, IconFactory::getIconForClassUid(class_uid));
    newItem->setPopupMenu(ContextMenuCollection::getInstance()->getRelockLementPopupMenu());
    m_patchesTree->addTopLevelItem(newItem);
    newItem->setData(0, Qt::UserRole, QVariant(uid));
    m_patchItems[p]->setExpanded(true);
}

void GuiElements::removeItemForPatch(Patch *p, const QString& uid)
{
    if(!m_patchItems.contains(p))
    {
        return ;
    }
    ContextMenuEnabledTreeWidgetItem* itm = m_patchItems[p];
    for(int i=0; i<itm->childCount(); i++)
    {
        QVariant v = itm->child(i)->data(0, Qt::UserRole);
        if(v.toString() == uid)
        {
            itm->removeChild(itm->child(i));
            return;
        }
    }
}
