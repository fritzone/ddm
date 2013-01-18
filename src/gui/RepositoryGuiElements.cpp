#include "RepositoryGuiElements.h"
#include "MainWindow.h"

RepositoryGuiElements::RepositoryGuiElements()
{

}

void RepositoryGuiElements::createGuiElements()
{
    m_repositoryTreeDock = new QDockWidget(QObject::tr("Repository"), MainWindow::instance());
    m_repositoryTreeDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_repositoryTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_repositoryTreeDock->setFloating(false);
    m_repositoryTreeDock->setMinimumSize(300, 340);
    m_repositoryTreeDock->setMaximumSize(QApplication::desktop()->screenGeometry().width() / 4, 9999);
    m_repositoryTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_repositoryTreeDock->resize(301,341);

    m_repositoryTree = new ContextMenuEnabledTreeWidget();
    m_repositoryTree->setAllColumnsShowFocus(true);
    m_repositoryTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_repositoryTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_repositoryTree->setColumnCount(1);
    m_repositoryTree->setHeaderHidden(false);

    QTreeWidgetItem *headerItm = m_repositoryTree->headerItem();
    headerItm->setText(0, QApplication::translate("MainWindow", "Name", 0, QApplication::UnicodeUTF8));
    m_repositoryTree->header()->setDefaultSectionSize(250);

    m_connectionsContextMenuHandler = new ContextMenuHandler();
    m_repositoryTree->setItemDelegate(new ContextMenuDelegate(m_connectionsContextMenuHandler, m_repositoryTree));
    m_repositoryTreeDock->setWidget(m_repositoryTree);

}
