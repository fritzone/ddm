#include "RepositoryGuiElements.h"
#include "MainWindow.h"

#include "core_Repository.h"
#include "db_DatabaseEngine.h"
#include "ContextMenuCollection.h"
#include "IconFactory.h"

RepositoryGuiElements::RepositoryGuiElements()
{
    m_repo = new Repository();
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

    // create the tree widget for "Databases"
    m_databasesTreeEntry = new ContextMenuEnabledTreeWidgetItem(0, QStringList(QObject::tr("Databases")));
    m_repositoryTree->addTopLevelItem(m_databasesTreeEntry);
    m_databasesTreeEntry->setIcon(0, IconFactory::getRepoDatabasesIcon());
    m_databasesTreeEntry->setPopupMenu(ContextMenuCollection::getInstance()->getRepositoryDatabasesPopupMenu());

    // create the tree widget for "Roles"
    m_rolesTreeEntry = new ContextMenuEnabledTreeWidgetItem(0, QStringList(QObject::tr("Roles")));
    m_repositoryTree->addTopLevelItem(m_rolesTreeEntry);
    m_rolesTreeEntry->setIcon(0, IconFactory::getRepoRoleIcon());
    m_rolesTreeEntry->setPopupMenu(ContextMenuCollection::getInstance()->getRepositoryDatabasesPopupMenu());

    // and populates them
    const QVector<DatabaseEngine*> & dbes = m_repo->getDatabases();
    for(int i=0; i<dbes.size(); i++)
    {
        createDatabaseeTreeEntry(dbes[i]);
    }
}


ContextMenuEnabledTreeWidgetItem* RepositoryGuiElements::createDatabaseeTreeEntry(DatabaseEngine* dbEngine)
{
    QStringList items;
    items << dbEngine->getDatabaseEngineName();

    ContextMenuEnabledTreeWidgetItem* newDbItem = new ContextMenuEnabledTreeWidgetItem(m_databasesTreeEntry, items);
    QVariant var(dbEngine->getDatabaseEngineName());
    newDbItem->setData(0, Qt::UserRole, var);
    newDbItem->setIcon(0, IconFactory::getRepoDatabasesIcon());
    m_repositoryTree->addTopLevelItem(newDbItem);

    return newDbItem ;
}
