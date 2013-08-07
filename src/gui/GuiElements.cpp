#include "GuiElements.h"
#include "MainWindow.h"
#include "Workspace.h"
#include "Solution.h"
#include "core_Patch.h"
#include "IconFactory.h"
#include "ContextMenuCollection.h"
#include "core_TableInstance.h"
#include "UidWarehouse.h"
#include "Version.h"

GuiElements::GuiElements() : m_projectTreeDock(0),m_issuesTreeDock(0),
    m_patchesTreeDock(0),
    m_projectTree(0), m_issuesTree(0), m_patchesTree(0),
    m_contextMenuHandler(0), m_issuesContextMenuHandler(0),
    m_patchItems()
{
}

void GuiElements::createGuiElements()
{
    m_contextMenuHandler = new ContextMenuHandler();
    // create the project tree dock window
    m_projectTreeDock = new QDockWidget(Workspace::getInstance()->currentSolution()->getName() + " / " + Workspace::getInstance()->currentSolution()->currentProject()->getName(), MainWindow::instance());
    m_projectTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_projectTreeDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_projectTreeDock->setFloating(false);
    m_projectTreeDock->setMinimumSize(300, 540);
    m_projectTreeDock->setMaximumSize(QApplication::desktop()->screenGeometry().width() / 4, 9999);
 //    m_projectTreeDock->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    m_projectTreeDock->resize(301,541);

    m_projectTree = new ContextMenuEnabledTreeWidget();
    m_projectTree->setAllColumnsShowFocus(true);
    m_projectTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_projectTree->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_projectTree->setItemDelegate(new ContextMenuDelegate(m_contextMenuHandler,m_projectTree));
    m_projectTree->setColumnCount(1);
    m_projectTree->setHeaderHidden(true);
    QObject::connect(m_projectTree, SIGNAL (itemClicked (QTreeWidgetItem*, int)),
                     MainWindow::instance(), SLOT(projectTreeItemClicked(QTreeWidgetItem*,int)));

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
    m_patchesTree->setColumnCount(2);
    m_patchesTree->setHeaderHidden(false);
    m_patchesTree->headerItem()->setText(1, QObject::tr("Status"));
    m_patchesTree->headerItem()->setText(0, QObject::tr("Element"));
    m_patchesTree->header()->setDefaultSectionSize(350);
    QObject::connect(m_patchesTree, SIGNAL (itemClicked ( QTreeWidgetItem*, int)),
                     MainWindow::instance(), SLOT(patchTreeItemClicked(QTreeWidgetItem*,int)));

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
    ContextMenuEnabledTreeWidgetItem* patchItem = new ContextMenuEnabledTreeWidgetItem(0, QStringList(p->getName()));
    patchItem->setIcon(0, IconFactory::getPatchIcon());
    patchItem->setPopupMenu(ContextMenuCollection::getInstance()->getSuspendPatchPopupMenu());
    m_patchesTree->addTopLevelItem(patchItem);
    patchItem->setData(0, Qt::UserRole, QVariant(p->getObjectUid()));
    m_patchItems[p] = patchItem;
    p->setLocation(patchItem);
    return patchItem;
}

ContextMenuEnabledTreeWidgetItem* GuiElements::updateItemForPatchWithState(Patch *p, const QString& class_uid, const QString &uid, const QString &name, int state)
{
    if(state == 1) // NEW stuff ! TODO: make this nicer
    {
        if(!m_patchItems.contains(p))
        {
            return 0;
        }

        for(int i=0; i<m_patchItems[p]->childCount(); i++)
        {
            QVariant v = m_patchItems[p]->child(i)->data(0, Qt::UserRole);
            if(v.toString() == uid)
            {
                m_patchItems[p]->child(i)->setIcon(1, IconFactory::getAddIcon());
                dynamic_cast<ContextMenuEnabledTreeWidgetItem*>(m_patchItems[p]->child(i))->setPopupMenu(ContextMenuCollection::getInstance()->getMenuForClassUid(class_uid));
            }
        }
    }
    if(state == 2) // RENAMED stuff ! TODO: make this ncier too!
    {
        if(!m_patchItems.contains(p))
        {
            return 0;
        }

        for(int i=0; i<m_patchItems[p]->childCount(); i++)
        {
            QVariant v = m_patchItems[p]->child(i)->data(0, Qt::UserRole);
            if(v.toString() == uid)
            {
                m_patchItems[p]->child(i)->setText(0, name);
            }
        }
    }
    if(state == 3) // DELETED stuff
    {
        if(!m_patchItems.contains(p))
        {
            return 0;
        }

        for(int i=0; i<m_patchItems[p]->childCount(); i++)
        {
            QVariant v = m_patchItems[p]->child(i)->data(0, Qt::UserRole);
            if(v.toString() == uid)
            {
                m_patchItems[p]->child(i)->setIcon(1, IconFactory::getRemoveIcon());
                dynamic_cast<ContextMenuEnabledTreeWidgetItem*>(m_patchItems[p]->child(i))->setPopupMenu(ContextMenuCollection::getInstance()->getUndeletePopupMenu());
                // TODO: create sub items with deleted state
                TableDeletionAction* tda = p->getTDA(uid);
                if(tda)
                {
                    for(int j=tda->deletedTable?0:1; j<tda->deletedTableInstances.size(); j++)
                    {
                        ContextMenuEnabledTreeWidgetItem* tdItem = new ContextMenuEnabledTreeWidgetItem(dynamic_cast<ContextMenuEnabledTreeWidgetItem*>(m_patchItems[p]->child(i)), QStringList(tda->deletedTableInstances.at(j)->getName()));
                        tdItem->setIcon(0, IconFactory::getTabinstIcon());
                        tdItem->setIcon(1, IconFactory::getRemoveIcon());
                        m_patchesTree->addTopLevelItem(tdItem);
                    }

                    m_patchItems[p]->child(i)->setExpanded(true);
                }
            }
        }
    }
    if(state == 4) // REMOVE stuff from the tree... but only if it was NOT unlocked before
    {
        if(!m_patchItems.contains(p))
        {
            return 0;
        }

        for(int i=0; i<m_patchItems[p]->childCount(); i++)
        {
            QVariant v = m_patchItems[p]->child(i)->data(0, Qt::UserRole);
            if(v.toString() == uid)
            {
                ObjectWithUid* obj = UidWarehouse::instance().getElement(uid);
                LockableElement* le = dynamic_cast<LockableElement*>(obj);
                if(le && le->lockState() == LockableElement::LOCKED)    // if the element was locked and deleted then "plain" undelete, ie, change the icon
                {
                    dynamic_cast<ContextMenuEnabledTreeWidgetItem*>(m_patchItems[p]->child(i))->setPopupMenu(ContextMenuCollection::getInstance()->getReLockMenuForClassUid(obj->getClassUid()));

                    m_patchItems[p]->child(i)->setIcon(1, IconFactory::getChangedIcon());
                    return 0;
                }
                else
                {
                    delete m_patchItems[p]->child(i);

                    if(m_patchItems[p]->childCount() == 0)
                    {
                        delete m_patchItems[p];
                        m_patchItems.remove(p);
                        if(m_patchItems.keys().size() == 0)
                        {
                            m_patchesTreeDock->hide();
                        }
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}

ContextMenuEnabledTreeWidgetItem* GuiElements::createNewItemForPatch(Patch *p, const QString& class_uid, const QString& uid, const QString& name)
{
    if(!m_patchItems.contains(p))
    {
        createNewPatchItem(p);
    }
    for(int i=0; i<m_patchItems[p]->childCount(); i++)
    {
        QVariant v = m_patchItems[p]->child(i)->data(0, Qt::UserRole);
        if(v.toString() == uid) // this element is already there
        {
            return dynamic_cast<ContextMenuEnabledTreeWidgetItem*>(m_patchItems[p]->child(i));
        }
    }
    ContextMenuEnabledTreeWidgetItem* newItem = new ContextMenuEnabledTreeWidgetItem(m_patchItems[p], QStringList(name));
    newItem->setIcon(0, IconFactory::getIconForClassUid(class_uid));
    newItem->setIcon(1, IconFactory::getChangedIcon());
    newItem->setPopupMenu(ContextMenuCollection::getInstance()->getReLockMenuForClassUid(class_uid));
    m_patchesTree->addTopLevelItem(newItem);
    newItem->setData(0, Qt::UserRole, QVariant(uid));
    m_patchItems[p]->setExpanded(true);
    m_uidToTreeItem[uid] = newItem;
    return newItem;
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
            ObjectWithUid* obj = UidWarehouse::instance().getElement(uid);
            LockableElement* le = dynamic_cast<LockableElement*>(obj);
            if(le && !le->lockState() == LockableElement::LOCKED)   // if the element was locked, and then deleted re-state the locked state entry in the tree
            {
                itm->child(i)->setIcon(1, IconFactory::getChangedIcon());
                dynamic_cast<ContextMenuEnabledTreeWidgetItem*>(itm->child(i))->setPopupMenu(ContextMenuCollection::getInstance()->getReLockMenuForClassUid(obj->getClassUid()));

                return ;
            }

            itm->removeChild(itm->child(i));
            break;
        }
    }

    if(itm->childCount() == 0)
    {
        delete itm;
        m_patchItems.remove(p);
        p->version()->removePatch(p);
    }

    if(m_patchItems.keys().size() == 0)
    {
        m_patchesTreeDock->hide();
    }
}

void GuiElements::populatePathcItem(ContextMenuEnabledTreeWidgetItem* /*patchItem*/, Patch *patch)
{
    // firstly the locked elements. This will NOT create tree items for the deleted ones
    const QStringList& lockeds = patch->getLockedUids();
    const QMap<QString, QString> & uidToClassUidMap = patch->getUidToClassUidMap();
    for(int i=0; i<lockeds.size(); i++)
    {
//        qDebug() << i;
        ObjectWithUid* ouid = UidWarehouse::instance().getElement(lockeds.at(i));
        if(ouid)
        {
            NamedItem* ni = dynamic_cast<NamedItem*>(ouid);
            if(ni)
            {
                createNewItemForPatch(patch, uidToClassUidMap[lockeds.at(i)], lockeds.at(i), ni->getName());
            }
        }
        else
        {
//            qDebug() << "no ouid for " << lockeds.at(i);
        }
    }

    // now the deleted items should be fixed with their icon
    const QStringList& deleteds = patch->getDeletedUids();
    for(int i=0; i<deleteds.size(); i++)
    {
        m_uidToTreeItem[deleteds.at(i)]->setIcon(1, IconFactory::getRemoveIcon());
        m_uidToTreeItem[deleteds.at(i)]->setPopupMenu(ContextMenuCollection::getInstance()->getUndeletePopupMenu());
    }
    // and their tree relationship should be built up
    const QMap<QString, QVector <QString> > & delS = patch->getDeletionStruct();
    for(int i=0; i<delS.keys().size(); i++)
    {
        QVector<QString> delUids = delS[delS.keys().at(i)];
        for(int j=0; j<delUids.size(); j++)
        {
            m_uidToTreeItem[delUids.at(j)]->parent()->removeChild(m_uidToTreeItem[delUids.at(j)]);
            m_uidToTreeItem[delS.keys().at(i)]->addChild(m_uidToTreeItem[delUids.at(j)]);
            m_uidToTreeItem[delUids.at(j)]->setData(0, Qt::UserRole, QVariant());
            m_uidToTreeItem[delUids.at(j)]->setPopupMenu(0);
        }
    }

    // the NEW items should be added
    const QStringList& news = patch->getNewUids();
    for(int i=0; i<news.size(); i++)
    {
        m_uidToTreeItem[news.at(i)]->setIcon(1, IconFactory::getAddIcon());
        m_uidToTreeItem[news.at(i)]->setPopupMenu(ContextMenuCollection::getInstance()->getMenuForClassUid(uidToClassUidMap[news.at(i)]));
    }

}
