#include "DefaultVersionImplementation.h"
#include "ContextMenuCollection.h"
#include "core_UserDataType.h"
#include "core_Table.h"
#include "core_Diagram.h"
#include "IconFactory.h"
#include "core_TableInstance.h"
#include "Project.h"
#include "ForeignKey.h"
#include "NameGenerator.h"
#include "VersionGuiElements.h"
#include "Workspace.h"
#include "Configuration.h"
#include "db_DatabaseEngine.h"
#include "Issue.h"
#include "IssueManager.h"
#include "core_View.h"
#include "core_Procedure.h"
#include "db_AbstractSQLGenerator.h"
#include "core_Trigger.h"
#include "core_Function.h"
#include "strings.h"
#include "UidWarehouse.h"
#include "Index.h"
#include "core_Patch.h"

// TODO: This is horrible!!!
#include "MainWindow.h"
#include "GuiElements.h"

#include <QtGui>

DefaultVersionImplementation::DefaultVersionImplementation(Project* p, int major, int minor)
    : Version(major, minor, p),
      version(""), m_data(), m_guiElements(0), m_validationFlags(0), m_patches(), m_currentPatchIndex(-1)
{
}

void DefaultVersionImplementation::createTreeItems(GuiElements* gui, ContextMenuEnabledTreeWidgetItem* projectItem, int idxAfter)
{
    qDebug() << this;
    m_guiElements = new VersionGuiElements(gui, this);
    m_guiElements->createGuiElements(projectItem, idxAfter);
}

void DefaultVersionImplementation::updateGui()
{
    QFont f = getGui()->getVersionItem()->font(0);

    if(lockState() == LockableElement::LOCKED)
    {
        f.setItalic(true);
        getGui()->getVersionItem()->setIcon(0, IconFactory::getLockedVersionIcon());
        getGui()->getVersionItem()->setPopupMenu(ContextMenuCollection::getInstance()->getReleasedVersionPopupMenu());
    }
    else
    if(lockState() == LockableElement::FINAL_LOCK)
    {
        f.setItalic(true);
        getGui()->getVersionItem()->setIcon(0, IconFactory::getFinalLockedVersionIcon());
        getGui()->getVersionItem()->setPopupMenu(ContextMenuCollection::getInstance()->getFinalisedVersionPopupMenu());

        getGui()->getVersionItem()->setForeground(0, QBrush(Qt::lightGray));
        getGui()->getTablesItem()->setForeground(0, QBrush(Qt::lightGray));
        getGui()->getTableInstancesItem()->setForeground(0, QBrush(Qt::lightGray));
        getGui()->getDiagramsItem()->setForeground(0, QBrush(Qt::lightGray));
        getGui()->getDtsItem()->setForeground(0, QBrush(Qt::lightGray));
        getGui()->getProceduresItem()->setForeground(0, QBrush(Qt::lightGray));
        getGui()->getFunctionsItem()->setForeground(0, QBrush(Qt::lightGray));
        getGui()->getTriggersItem()->setForeground(0, QBrush(Qt::lightGray));
        getGui()->getViewsItem()->setForeground(0, QBrush(Qt::lightGray));

        if(getGui()->hasIntsDtsItem()) getGui()->getIntsDtsItem()->setForeground(0, QBrush(Qt::lightGray));
        if(getGui()->hasStringDtsItem()) getGui()->getStringDtsItem()->setForeground(0, QBrush(Qt::lightGray));
        if(getGui()->hasDateDtsItem()) getGui()->getDateDtsItem()->setForeground(0, QBrush(Qt::lightGray));
        if(getGui()->hasBlobDtsItem()) getGui()->getBlobDtsItem()->setForeground(0, QBrush(Qt::lightGray));
        if(getGui()->hasBoolDtsItem()) getGui()->getBoolDtsItem()->setForeground(0, QBrush(Qt::lightGray));
        if(getGui()->hasMiscDtsItem()) getGui()->getMiscDtsItem()->setForeground(0, QBrush(Qt::lightGray));
        if(getGui()->hasSpatialDtsItem()) getGui()->getSpatialDtsItem()->setForeground(0, QBrush(Qt::lightGray));


    }
    else
    {
        f.setItalic(false);
        getGui()->getVersionItem()->setIcon(0, IconFactory::getVersionIcon());
        getGui()->getVersionItem()->setPopupMenu(ContextMenuCollection::getInstance()->getMajorVersionPopupMenu());
    }

    getGui()->getVersionItem()->setFont(0, f);
    getGui()->getTablesItem()->setFont(0, f);
    getGui()->getTableInstancesItem()->setFont(0, f);
    getGui()->getDiagramsItem()->setFont(0, f);
    getGui()->getDtsItem()->setFont(0, f);
    getGui()->getProceduresItem()->setFont(0, f);
    getGui()->getFunctionsItem()->setFont(0, f);
    getGui()->getTriggersItem()->setFont(0, f);
    getGui()->getViewsItem()->setFont(0, f);

    if(getGui()->hasIntsDtsItem()) getGui()->getIntsDtsItem()->setFont(0, f);
    if(getGui()->hasStringDtsItem()) getGui()->getStringDtsItem()->setFont(0, f);
    if(getGui()->hasDateDtsItem()) getGui()->getDateDtsItem()->setFont(0, f);
    if(getGui()->hasBlobDtsItem()) getGui()->getBlobDtsItem()->setFont(0, f);
    if(getGui()->hasBoolDtsItem()) getGui()->getBoolDtsItem()->setFont(0, f);
    if(getGui()->hasMiscDtsItem()) getGui()->getMiscDtsItem()->setFont(0, f);
    if(getGui()->hasSpatialDtsItem()) getGui()->getSpatialDtsItem()->setFont(0, f);

}

void DefaultVersionImplementation::addNewDataType(UserDataType* dt, bool initial)
{
    int i = 0;
    while(i < m_data.m_dataTypes.size())
    {
        if(m_data.m_dataTypes.at(i)->getType() != dt->getType()) i++;
        else break;
    }

    m_data.m_dataTypes.insert(i, dt);

    if(lockState() == LockableElement::LOCKED && !initial)
    {
        MainWindow::instance()->createPatchElement(this, dt, dt->getObjectUid(), false);
        getWorkingPatch()->addNewElement(dt->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, dt, dt->getObjectUid(), 1);
    }


}

bool DefaultVersionImplementation::hasDataType(const QString& name) const
{
    for(int i=0; i < m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == name)
        {
            return true;
        }
    }
    return false;
}

UserDataType* DefaultVersionImplementation::getDataType(const QString& name)
{
    for(int i=0; i< m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == name)
        {
            return const_cast<UserDataType*>(m_data.m_dataTypes.at(i));  // yeah, this sucks
        }
    }
    return 0;
}

int DefaultVersionImplementation::getDataTypeIndex(const QString& name)
{
    for(int i=0; i < m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == name)
        {
            return i;
        }
    }
    return -1;
}

inline void DefaultVersionImplementation::addTable(Table *t, bool initial)
{
    m_data.m_tables.append(t);
    if(lockState() == LockableElement::LOCKED && !initial)
    {
        MainWindow::instance()->createPatchElement(this, t, t->getObjectUid(), false);
        getWorkingPatch()->addNewElement(t->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, t, t->getObjectUid(), 1);
    }
}

inline void DefaultVersionImplementation::addDiagram(Diagram* d, bool initial)
{
    m_data.m_diagrams.append(d);

    if(lockState() == LockableElement::LOCKED && !initial)
    {
        MainWindow::instance()->createPatchElement(this, d, d->getObjectUid(), false);
        getWorkingPatch()->addNewElement(d->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, d, d->getObjectUid(), 1);
    }

}

inline bool DefaultVersionImplementation::hasTable(Table *t)
{
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
//        qDebug() << m_data.m_tables.at(i)->getObjectUid() << "==" << t->getObjectUid() << " IN " <<getVersionText();
        if(m_data.m_tables.at(i)->getObjectUid() == t->getObjectUid())
        {
            return true;
        }
    }
    return false;
}

inline bool DefaultVersionImplementation::hasTable(const QString& tb)
{
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        if(QString::compare(m_data.m_tables[i]->getName(), tb, Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }
    return false;
}

Table* DefaultVersionImplementation::getTable(const QString &name) const
{
    for(int i=0; i< m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getName().toUpper() == name.toUpper())
        {
            return m_data.m_tables[i];
        }
    }
    return 0;
}

Table* DefaultVersionImplementation::getTableWithUid(const QString& uid) const
{
    for(int i=0; i< m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getObjectUid() == uid)
        {
            return m_data.m_tables[i];
        }
    }
    return 0;
}

Procedure* DefaultVersionImplementation::getProcedureWithUid(const QString& uid) const
{
    for(int i=0; i<m_data.m_procedures.size(); i++)
    {
        if(m_data.m_procedures[i]->getObjectUid() == uid)
        {
            return m_data.m_procedures[i];
        }
    }
    return 0;
}

Function* DefaultVersionImplementation::getFunctionWithUid(const QString& uid) const
{
    for(int i=0; i<m_data.m_functions.size(); i++)
    {
        if(m_data.m_functions[i]->getObjectUid() == uid)
        {
            return m_data.m_functions[i];
        }
    }
    return 0;
}

UserDataType* DefaultVersionImplementation::getUserDataTypeWithUid(const QString& uid) const
{
    for(int i=0; i<m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes[i]->getObjectUid() == uid)
        {
            return m_data.m_dataTypes[i];
        }
    }

    return 0;

}

Trigger* DefaultVersionImplementation::getTriggerWithUid(const QString& uid) const
{
    for(int i=0; i<m_data.m_triggers.size(); i++)
    {
        if(m_data.m_triggers[i]->getObjectUid() == uid)
        {
            return m_data.m_triggers[i];
        }
    }

    return 0;
}

View* DefaultVersionImplementation::getViewWithUid(const QString& uid) const
{
    for(int i=0; i<m_data.m_views.size(); i++)
    {
        if(m_data.m_views[i]->getObjectUid() == uid)
        {
            return m_data.m_views[i];
        }
    }

    return 0;
}

Diagram* DefaultVersionImplementation::getDiagramWithUid(const QString& uid) const
{
    for(int i=0; i<m_data.m_diagrams.size(); i++)
    {
        if(m_data.m_diagrams[i]->getObjectUid() == uid)
        {
            return m_data.m_diagrams[i];
        }
    }

    return 0;
}

Diagram* DefaultVersionImplementation::getDiagram(const QString& name)
{
    for(int i=0; i<m_data.m_diagrams.size(); i++)
    {
        if(m_data.m_diagrams[i]->getName() == name)
        {
            return m_data.m_diagrams[i];
        }
    }

    return 0;
}

inline const QVector<UserDataType*>& DefaultVersionImplementation::getDataTypes() const
{
    return m_data.m_dataTypes;
}

inline const QVector<Table*>& DefaultVersionImplementation::getTables() const
{
    return m_data.m_tables;
}

void DefaultVersionImplementation::purgeSentencedTableInstances()
{
    int i=0;
    while(i<m_data.m_tableInstances.size())
    {
        if(m_data.m_tableInstances.at(i)->sentenced())
        {
            m_data.m_tableInstances.remove(i);
        }
        else
        {
            i++;
        }
    }
}

void DefaultVersionImplementation::deleteTableInstance(TableInstance *tinst, TableDeletionAction *&tda)
{
    doDeleteTableInstance(tinst, tda);
}

void DefaultVersionImplementation::doDeleteTableInstance(TableInstance *tinst, TableDeletionAction*& tda)
{
    if(tda)
    {
        if(tda->deletedTableInstances.indexOf(tinst) == -1)
        {
            tda->deletedTableInstances.append(tinst);
//            qDebug() << "DELETE for patch " << tinst->getName();
        }
    }

    tinst->sentence();
    // now find which table instances were created because of this and remove them
    QVector<TableInstance*>& insted = tinst->getInstantiatedTableInstances();
    for(int i=0; i<insted.size(); i++)
    {
        // except if there are other table instances that have it as instantiated
        bool someoneElseAlsoInstantiatedThis = false;
        for(int j=0; j<m_data.m_tableInstances.size(); j++)
        {
            if(tinst != m_data.m_tableInstances.at(j) )
            {
                for(int k=0; k<m_data.m_tableInstances.at(j)->getInstantiatedTableInstances().size(); k++)
                {
                    if(m_data.m_tableInstances.at(j)->getInstantiatedTableInstances().at(k) == insted.at(i))
                    {
                        someoneElseAlsoInstantiatedThis = true;
                        break;
                    }
                }
                if(someoneElseAlsoInstantiatedThis)
                {
                    break;
                }
            }
        }
        if(!someoneElseAlsoInstantiatedThis)
        {
            if(!insted.at(i)->sentenced() && insted.at(i)->instantiatedBecuaseOfRkReference())
            {
                if(getWorkingPatch()->elementWasLockedInThisPatch(insted.at(i)->getObjectUid()))
                {
                    QMessageBox::warning(0, QObject::tr("Locked Table Instance found"),
                                         QObject::tr("Cannot execute the deletion operation since the following table instance is already unlocked in the patch:<BR><UL><LI>") +
                                         insted.at(i)->getName() +
                                         QObject::tr("</LI></UL><BR>Firstly <B>Re-lock</B> (right click -> Lock) the table instance then delete the table."), QMessageBox::Ok);
                    tinst->unSentence();
//                    qDebug() << "AB";
                    delete tda;
                    tda = 0;
                    return;

                }

                deleteTableInstance(insted.at(i), tda);
            }
        }
    }

    for(int i=0; i<getTableInstances().size(); i++)
    {
        if(getTableInstances().at(i)->sentenced())
        {
            QString uid = getTableInstances().at(i)->getObjectUid();
            ContextMenuEnabledTreeWidgetItem* tabInstItem = getGui()->getTableInstancesItem();
            for(int j=0; j<tabInstItem->childCount(); j++)
            {
                QVariant a = tabInstItem->child(j)->data(0, Qt::UserRole);
                if(a.toString() == uid)
                {
                    delete tabInstItem->child(j);
                }
            }

            ContextMenuEnabledTreeWidgetItem* sqlItem = getGui()->getFinalSqlItem();
            for(int j=0; j<sqlItem->childCount(); j++)
            {
                QVariant a = sqlItem->child(j)->data(0, Qt::UserRole);
                if(a.toString() == uid)
                {
                    delete sqlItem->child(j);
                }
            }
        }
    }

    // and finally remove all purged table instances
    purgeSentencedTableInstances();
}


void DefaultVersionImplementation::deleteTableInstance(TableInstance *tinst)
{
    TableDeletionAction* tda = new TableDeletionAction(0);

    doDeleteTableInstance(tinst, tda);
    if(tda == 0) return;

    if(lockState() == LockableElement::LOCKED)  // remove the entry from the patch
    {
        // check if this table instance was created as a NEW one in this patch
        if(getWorkingPatch()->elementWasNewInThisPatch(tinst->getObjectUid()))
        {
            getWorkingPatch()->removeNewElementBecauseOfDeletion(tinst->getObjectUid());
            MainWindow::instance()->updatePatchElementToReflectState(this, tinst, tinst->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
        }
        else
        {
            MainWindow::instance()->createPatchElement(this, tinst, tinst->getObjectUid(), false);
            getWorkingPatch()->markElementForDeletion(tinst->getObjectUid());
            getWorkingPatch()->addDeletedTable(tinst->getObjectUid(), tda);
            MainWindow::instance()->updatePatchElementToReflectState(this, tinst, tinst->getObjectUid(), 3); // 3 is DELETED
        }

    }
}

bool DefaultVersionImplementation::deleteTable(Table *tab)
{
    int tabIndex = m_data.m_tables.indexOf(tab);
    if(tabIndex == -1)
    {
        return false;
    }

    QString incomingForeignKeys = "";

    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getForeignKeyToTable(tab->getName()) != 0)
        {
            incomingForeignKeys+= "\n - " + m_data.m_tables[i]->getName();
        }
    }

    if(incomingForeignKeys.length() > 0)
    {
        QMessageBox::warning(0, QObject::tr("Foreign keys found"),
                             QObject::tr("Cannot delete this table since the following tables are referencing it through a foreign key: ") +
                             incomingForeignKeys +
                             QObject::tr("\nFirstly remove the foreign keys, then delete the table."), QMessageBox::Ok);
        return false;
    }

    for(int i=0; i<m_data.m_diagrams.size(); i++)
    {
        // TODO: on a delete create a solution for the diagrams too, so that they can be restored
        m_data.m_diagrams[i]->removeTable(tab->getName());
    }

    TableDeletionAction *tda = new TableDeletionAction(tab);

    // and now we should delete the table instances that were created based on this table, and the SQLs too...
    int i=0;
    while(i<m_data.m_tableInstances.size())
    {
        if(m_data.m_tableInstances.at(i)->table()->getName() == tab->getName())
        {
            TableInstance* tinst = m_data.m_tableInstances.at(i);
            if(getWorkingPatch()->elementWasLockedInThisPatch(tinst->getObjectUid()))
            {
                QMessageBox::warning(0, QObject::tr("Locked Table Instance found"),
                                     QObject::tr("Cannot execute the deletion operation since the following table instance is already unlocked in the patch:<BR><UL><LI>") +
                                     tinst->getName() +
                                     QObject::tr("</LI></UL><BR>Firstly <B>Re-lock</B> (right click -> Lock) the table instance then delete the table."), QMessageBox::Ok);
                delete tda;
                return false;

            }
            deleteTableInstance(tinst, tda);
            if(tda == 0)
            {
                return false;
            }
            // check if this table instance was created as a NEW one in this patch
            if(getWorkingPatch()->elementWasNewInThisPatch(tinst->getObjectUid()))
            {
                getWorkingPatch()->removeNewElementBecauseOfDeletion(tinst->getObjectUid());
                MainWindow::instance()->updatePatchElementToReflectState(this, tinst, tinst->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
            }
        }
        else
        {
            i++;
        }
    }

    // BIG TODO: see if this table is used in any of the triggers/diagrams/views/functions/procedures ...

    // and delete the issues of other tables, this table was referenced in
    QVector<Issue*> issuesReferencingTheTable = IssueManager::getInstance().getIssuesReferencingTable(tab->getName());
    for(int i=0; i<issuesReferencingTheTable.size(); i++)
    {
        removeIssue(issuesReferencingTheTable .at(i)->getName());
        IssueManager::getInstance().ignoringIssue(issuesReferencingTheTable.at(i));
    }
    // now delete all the issues this table is referenced in
    QVector<Issue*> issuesOfTheTable = IssueManager::getInstance().getIssuesOfTable(tab->getName());
    for(int i=0; i<issuesOfTheTable.size(); i++)
    {
        removeIssue(issuesOfTheTable.at(i)->getName());
        IssueManager::getInstance().ignoringIssue(issuesOfTheTable.at(i));
    }
    getGui()->cleanupOrphanedIssueTableItems();

    // remove from the parenttable
    if(tab->getParent())
    {
        tda->parentTable = tab->getParent();
        tab->getParent()->removeSpecializedTable(tab);
    }

    // tree cleanup

    // remove the DOC item
    tab->onDeleteDoc();
    delete tab->getLocation();

    // remove from inside
    m_data.m_tables.remove(tabIndex);

    if(lockState() == LockableElement::LOCKED)  // marking the element as deleted, ro removing
    {
        if(!getWorkingPatch()->elementWasNewInThisPatch(tab->getObjectUid())) // but only if it was NOT a newly created element
        {
            MainWindow::instance()->createPatchElement(this, tab, tab->getObjectUid(), false);
            getWorkingPatch()->markElementForDeletion(tab->getObjectUid());
            getWorkingPatch()->addDeletedTable(tab->getObjectUid(), tda);
            MainWindow::instance()->updatePatchElementToReflectState(this, tab, tab->getObjectUid(), 3); // 3 is DELETED
        }
        else
        {
            getWorkingPatch()->removeNewElementBecauseOfDeletion(tab->getObjectUid());
            MainWindow::instance()->updatePatchElementToReflectState(this, tab, tab->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
        }
    }
    return true;
}



void DefaultVersionImplementation::removeForeignKeyFromDiagrams(ForeignKey* fkToRemove)
{
    for(int i=0; i<m_data.m_diagrams.size(); i++)
    {
        m_data.m_diagrams[i]->forcefullyRemoveForeignKey(fkToRemove);
    }
}

Table* DefaultVersionImplementation::duplicateTable(Table *src)
{
    Table* dup = new Table(*src);
    dup->setForcedUid(QUuid::createUuid().toString());
    UidWarehouse::instance().addElement(dup, this);

    dup->setName(NameGenerator::getUniqueName(this,  (itemGetter)&Version::getTable, src->getName()+"_copy"));
    addTable(dup, false);
    return dup;
}

void DefaultVersionImplementation::setupTableParentChildRelationships()
{
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getTempTabName().length() > 0)
        {
            m_data.m_tables[i]->setParent(getTable(m_data.m_tables[i]->getTempTabName()));
        }
    }
}

TableInstance* DefaultVersionImplementation::instantiateTable(Table* tab, bool becauseOfReference)
{
    TableInstance* tabInst = new TableInstance(tab, becauseOfReference, QUuid::createUuid().toString(), this);
    TableInstance* other = getTableInstance(tabInst->getName());        // just to see that we have another instance of this table
    if(other && other != tabInst)
    {   // and in this case generate a new name for the current one
        tabInst->setName(NameGenerator::getUniqueName(this,  (itemGetter)&Version::getTableInstance, tabInst->getName()));
    }
    m_data.m_tableInstances.append(tabInst);
    if(lockState() == LockableElement::LOCKED)
    {
        MainWindow::instance()->createPatchElement(this, tabInst, tabInst->getObjectUid(), false);
        getWorkingPatch()->addNewElement(tabInst->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, tabInst, tabInst->getObjectUid(), 1);

    }
    return tabInst;
}

TableInstance* DefaultVersionImplementation::getTableInstance(const QString& name) const
{
    for(int i=0; i<m_data.m_tableInstances.size(); i++)
    {
        if(m_data.m_tableInstances.at(i)->getName() == name)
        {
            return m_data.m_tableInstances.at(i);
        }
    }
    return 0;
}

TableInstance* DefaultVersionImplementation::getTableInstanceWithUid(const QString& uid) const
{
    for(int i=0; i<m_data.m_tableInstances.size(); i++)
    {
        if(m_data.m_tableInstances.at(i)->getObjectUid() == uid)
        {
            return m_data.m_tableInstances.at(i);
        }
    }
    return 0;
}


void DefaultVersionImplementation::deleteDataType(const QString& dtName)
{
    for(int i=0; i<m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == dtName)
        {
            UserDataType* udt = m_data.m_dataTypes.at(i);
            m_data.m_dataTypes.remove(i);

            DataTypeDeletionAction* dtda = new DataTypeDeletionAction;
            dtda->deletedDataType = udt;

            if(lockState() == LockableElement::LOCKED)  // marking the element as deleted, ro removing
            {
                if(!getWorkingPatch()->elementWasNewInThisPatch(udt->getObjectUid())) // but only if it was NOT a newly created element
                {
                    MainWindow::instance()->createPatchElement(this, udt, udt->getObjectUid(), false);
                    getWorkingPatch()->markElementForDeletion(udt->getObjectUid());
                    getWorkingPatch()->addDeletedDataType(udt->getObjectUid(), dtda);
                    MainWindow::instance()->updatePatchElementToReflectState(this, udt, udt->getObjectUid(), 3); // 3 is DELETED
                }
                else
                {
                    getWorkingPatch()->removeNewElementBecauseOfDeletion(udt->getObjectUid());
                    MainWindow::instance()->updatePatchElementToReflectState(this, udt, udt->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
                }
            }

        }
    }
}

UserDataType* DefaultVersionImplementation::duplicateDataType(const QString& name)
{
    UserDataType* src = 0;
    for(int i=0; i<m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == name)
        {
            src = m_data.m_dataTypes.at(i);
            break;
        }
    }
    if(!src)
    {
        return 0;
    }
    UserDataType* dt = new UserDataType(*src);
    dt->setName(NameGenerator::getUniqueName(this,  (itemGetter)&Version::getDataType, dt->getName()));
    addNewDataType(dt, false);
    return dt;
}

void DefaultVersionImplementation::deleteView(const QString &name)
{
    View* v = getView(name);
    m_data.m_views.remove(m_data.m_views.indexOf(v));
    delete v->getLocation();
    delete v->getSqlLocation();
    ViewDeletionAction* vda = new ViewDeletionAction;
    vda->deletedView = v;

    if(lockState() == LockableElement::LOCKED)  // marking the element as deleted, ro removing
    {
        if(!getWorkingPatch()->elementWasNewInThisPatch(v->getObjectUid())) // but only if it was NOT a newly created element
        {
            MainWindow::instance()->createPatchElement(this, v, v->getObjectUid(), false);
            getWorkingPatch()->markElementForDeletion(v->getObjectUid());
            getWorkingPatch()->addDeletedView(v->getObjectUid(), vda);
            MainWindow::instance()->updatePatchElementToReflectState(this, v, v->getObjectUid(), 3); // 3 is DELETED
        }
        else
        {
            getWorkingPatch()->removeNewElementBecauseOfDeletion(v->getObjectUid());
            MainWindow::instance()->updatePatchElementToReflectState(this, v, v->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
        }
    }
}

void DefaultVersionImplementation::deleteFunction(const QString& f)
{
    Function* func = getFunction(f);
    m_data.m_functions.remove(m_data.m_functions.indexOf(func));
    delete func->getLocation();
    delete func->getSqlLocation();
    FunctionDeletionAction* pda = new FunctionDeletionAction;
    pda->deletedFunction = func;

    if(lockState() == LockableElement::LOCKED)  // marking the element as deleted, ro removing
    {
        if(!getWorkingPatch()->elementWasNewInThisPatch(func->getObjectUid())) // but only if it was NOT a newly created element
        {
            MainWindow::instance()->createPatchElement(this, func, func->getObjectUid(), false);
            getWorkingPatch()->markElementForDeletion(func->getObjectUid());
            getWorkingPatch()->addDeletedFunction(func->getObjectUid(), pda);
            MainWindow::instance()->updatePatchElementToReflectState(this, func, func->getObjectUid(), 3); // 3 is DELETED
        }
        else
        {
            getWorkingPatch()->removeNewElementBecauseOfDeletion(func->getObjectUid());
            MainWindow::instance()->updatePatchElementToReflectState(this, func, func->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
        }
    }
}

void DefaultVersionImplementation::deleteTrigger(const QString& t)
{
    Trigger* trg = getTrigger(t);
    m_data.m_triggers.remove(m_data.m_triggers.indexOf(trg));
    delete trg->getLocation();
    delete trg->getSqlLocation();
    TriggerDeletionAction* tda = new TriggerDeletionAction;
    tda->deletedTrigger= trg;

    // TODO: this is more or less a duplication with other places. Fix it.
    if(lockState() == LockableElement::LOCKED)  // marking the element as deleted, ro removing
    {
        if(!getWorkingPatch()->elementWasNewInThisPatch(trg->getObjectUid())) // but only if it was NOT a newly created element
        {
            MainWindow::instance()->createPatchElement(this, trg, trg->getObjectUid(), false);
            getWorkingPatch()->markElementForDeletion(trg->getObjectUid());
            getWorkingPatch()->addDeletedTrigger(trg->getObjectUid(), tda);
            MainWindow::instance()->updatePatchElementToReflectState(this, trg, trg->getObjectUid(), 3); // 3 is DELETED
        }
        else
        {
            getWorkingPatch()->removeNewElementBecauseOfDeletion(trg->getObjectUid());
            MainWindow::instance()->updatePatchElementToReflectState(this, trg, trg->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
        }
    }
}

void DefaultVersionImplementation::deleteProcedure(const QString& p)
{
    Procedure* v = getProcedure(p);
    m_data.m_procedures.remove(m_data.m_procedures.indexOf(v));
    delete v->getLocation();
    delete v->getSqlLocation();
    ProcedureDeletionAction* pda = new ProcedureDeletionAction;
    pda->deletedProcedure = v;

    if(lockState() == LockableElement::LOCKED)  // marking the element as deleted, ro removing
    {
        if(!getWorkingPatch()->elementWasNewInThisPatch(v->getObjectUid())) // but only if it was NOT a newly created element
        {
            MainWindow::instance()->createPatchElement(this, v, v->getObjectUid(), false);
            getWorkingPatch()->markElementForDeletion(v->getObjectUid());
            getWorkingPatch()->addDeletedProcedure(v->getObjectUid(), pda);
            MainWindow::instance()->updatePatchElementToReflectState(this, v, v->getObjectUid(), 3); // 3 is DELETED
        }
        else
        {
            getWorkingPatch()->removeNewElementBecauseOfDeletion(v->getObjectUid());
            MainWindow::instance()->updatePatchElementToReflectState(this, v, v->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
        }
    }
}

void DefaultVersionImplementation::deleteDiagram(const QString& name)
{
    Diagram* dgr = getDiagram(name);
    m_data.m_diagrams.remove(m_data.m_diagrams.indexOf(dgr));
    delete dgr->getLocation();
    DiagramDeletionAction* dda = new DiagramDeletionAction;
    dda->deletedDiagram = dgr;

    if(lockState() == LockableElement::LOCKED)  // marking the element as deleted, ro removing
    {
        if(!getWorkingPatch()->elementWasNewInThisPatch(dgr->getObjectUid())) // but only if it was NOT a newly created element
        {
            MainWindow::instance()->createPatchElement(this, dgr, dgr->getObjectUid(), false);
            getWorkingPatch()->markElementForDeletion(dgr->getObjectUid());
            getWorkingPatch()->addDeletedDiagram(dgr->getObjectUid(), dda);
            MainWindow::instance()->updatePatchElementToReflectState(this, dgr, dgr->getObjectUid(), 3); // 3 is DELETED
        }
        else
        {
            getWorkingPatch()->removeNewElementBecauseOfDeletion(dgr->getObjectUid());
            MainWindow::instance()->updatePatchElementToReflectState(this, dgr, dgr->getObjectUid(), 4); // 4 is REMOVE FROM THE TREE
        }
    }
}

QVector<Table*> DefaultVersionImplementation::getTablesReferencingAColumnThroughForeignKeys(const Column* col)
{
    QVector<Table*> result;
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        for(int j=0; j<m_data.m_tables.at(i)->getForeignKeys().size(); j++)
        {
            if(m_data.m_tables.at(i)->getForeignKeys().at(j)->foreignColumns().contains(col))
            {
                result.append(m_data.m_tables.at(i));
                break;
            }
        }
    }
    return result;
}

void DefaultVersionImplementation::setupForeignKeyRelationshipsForATable(Table* tab)
{
    for(int j=0; j<tab->getForeignKeys().size(); j++)
    {
        ForeignKey* fkJ = tab->getForeignKeys()[j];
        for(int k=0; k<fkJ->getAssociations().size(); k++)
        {
            ForeignKey::ColumnAssociation* assK = fkJ->getAssociation(k);
            // first: set the tables
            for(int l=0; l<getTables().size(); l++)
            {
                if(getTables().at(l)->getName() == assK->getSForeignTable())
                {
                    assK->setForeignTable(getTables().at(l));
                }
                assK->setLocalTable(tab);
                //if(getTables().at(l)->getName() == assK->getSLocalTable())
                //{
                //    assK->setLocalTable(getTables().at(l));
                //}
            }
            // then: set the columns of those tables
            for(int l=0; l<assK->getLocalTable()->columns().size(); l++)
            {
                Column* colL = assK->getLocalTable()->getColumns().at(l);
                if(colL->getName() == assK->getSLocalColumn())
                {
                    assK->setLocalColumn(colL);
                }
            }
            for(int l=0; l<assK->getForeignTable()->columns().size(); l++)
            {
                Column* colL = assK->getForeignTable()->getColumns().at(l);
                if(colL->getName() == assK->getSForeignColumn())
                {
                    assK->setForeignColumn(colL);
                }
            }
        }
    }
}

QList<QString> DefaultVersionImplementation::getSqlScript(bool generateDelimiters, const Connection* dest, QStringList& uids)
{
    QList<QString> finalSql;
    QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
    opts["FKSposition"] = "OnlyInternal";
    opts["PKSposition"] = "AfterColumnsDeclaration";
//    bool comments =  opts.contains("Comments") && opts["Comments"] == "Yes";

    if(Workspace::getInstance()->currentProjectIsOop())   // list the table instances' SQL
    {
        bool fkAllowed = true;
        for(int i=0; i<getTableInstances().size(); i++)
        {
            QStringList sql = getTableInstances().at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts, dest);
            finalSql << sql;
            for(int j=0; j<sql.size(); j++) uids << getTableInstances().at(i)->getObjectUid();

            if(fkAllowed) fkAllowed = !m_project->getEngine()->tableBlocksForeignKeyFunctionality(getTableInstances().at(i)->table());
        }

        if(fkAllowed)
        {
            for(int i=0; i<getTableInstances().size(); i++)
            {
                QStringList foreignKeyCommands = m_project->getEngine()->getSqlGenerator()->generateAlterTableForForeignKeys(getTableInstances().at(i)->table(), opts);

                finalSql << foreignKeyCommands;
                for(int j=0; j<foreignKeyCommands.size(); j++) uids << getTableInstances().at(i)->getObjectUid();
            }
        }
    }
    else    // list table's SQL
    {
        bool fkAllowed = true;
        for(int i=0; i<getTables().size(); i++)
        {
            QStringList sql = getTables().at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts, dest);
            finalSql << sql;
            for(int j=0; j<sql.size(); j++) uids << getTables().at(i)->getObjectUid();
            if(fkAllowed)
            {
                fkAllowed =!m_project->getEngine()->tableBlocksForeignKeyFunctionality(getTables().at(i));
            }
        }

        if(fkAllowed)
        {
            for(int i=0; i<getTables().size(); i++)
            {
                QStringList foreignKeyCommands = m_project->getEngine()->getSqlGenerator()->generateAlterTableForForeignKeys(getTables().at(i), opts);
                finalSql << foreignKeyCommands;
                for(int j=0; j<foreignKeyCommands.size(); j++) uids << getTables().at(i)->getObjectUid();
            }
        }
        else
        {
//            qDebug() << "FKs not allowed";
        }
    }

    // and the views
//    if(comments)
//    {   finalSql << "-- Creating the views\n";
//        finalSql.append("\n");
//    }
    for(int i=0; i<m_data.m_views.size(); i++)
    {
        QStringList t = m_data.m_views.at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts, dest);
        QString s = "";
        for(int j=0; j<t.size(); j++)
        {
            s += t.at(j) + " ";
        }
        s += ";\n\n";
        finalSql << s;
        uids << m_data.m_views.at(i)->getObjectUid();
    }

    // and the procedures
    finalSql.append("\n");

//    if(m_data.m_procedures.size() && comments)
//    {
//        finalSql << "-- Creating the procedures\n";
//        finalSql.append("\n");
//    }

    if(m_data.m_procedures.size() && generateDelimiters)
    {
        finalSql << m_project->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
        uids << nullUid;
    }

    for(int i=0; i<m_data.m_procedures.size(); i++)
    {
        QString s = "";
        //if (comments) s = "-- Procedure " + m_data.m_procedures.at(i)->getName() + strNewline;
        QStringList t = m_data.m_procedures.at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts, dest);
        for(int j=0; j<t.size(); j++)
        {
            s += t.at(j) + " ";
        }
        if(generateDelimiters)
        {
            s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
        }
        else
        {
            s += strSemicolon + strNewline;
        }
        uids << m_data.m_procedures.at(i)->getObjectUid();
        finalSql << s;
    }


    if(m_data.m_procedures.size() && generateDelimiters)
    {
        finalSql << m_project->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
        uids << nullUid;

    }

    // and the functions
    finalSql.append("\n");
    if(m_data.m_functions.size() && generateDelimiters)
    {
        finalSql << m_project->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
        uids << nullUid;

    }

//    if(m_data.m_functions.size() && comments)
//    {
//        finalSql << "-- Creating the functions";
//        finalSql.append("\n");
//    }

    for(int i=0; i<m_data.m_functions.size(); i++)
    {
        QString s = "";
        //if (comments) s = "-- Function " + m_data.m_functions.at(i)->getName() + strNewline;
        QStringList t = m_data.m_functions.at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts, dest);
        for(int j=0; j<t.size(); j++)
        {
            s += t.at(j) + " ";
        }
        if(generateDelimiters)
        {
            s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
        }
        else
        {
            s += strSemicolon + strNewline;
        }
        finalSql << s;
        uids << m_data.m_functions.at(i)->getObjectUid();
    }

    if(m_data.m_functions.size() && generateDelimiters)
    {
        finalSql << m_project->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
        uids << nullUid;
    }

    // and the triggers
//    finalSql.append("\n");

//    if(m_data.m_triggers.size() && comments)
//    {
//        finalSql << "-- Creating the triggers\n";
//        finalSql.append("\n");
//    }

    for(int i=0; i<m_data.m_triggers.size(); i++)
    {
        QString s = "";
        //if (comments) s = "-- Trigger " + m_data.m_triggers.at(i)->getName() + strNewline;
        QStringList t = m_data.m_triggers.at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts, dest);
        for(int j=0; j<t.size(); j++)
        {
            s += t.at(j) + " ";
        }
        finalSql << s;
        uids << m_data.m_triggers.at(i)->getObjectUid();
    }

    return finalSql;
}

const QVector<Diagram*>& DefaultVersionImplementation::getDiagrams() const
{
    return m_data.m_diagrams;
}

const QVector<View*>& DefaultVersionImplementation::getViews()
{
    return m_data.m_views;
}

void DefaultVersionImplementation::addTableInstance(TableInstance* inst, bool initial)
{
    m_data.m_tableInstances.append(inst);
    if(lockState() == LockableElement::LOCKED && !initial)
    {
        MainWindow::instance()->createPatchElement(this, inst, inst->getObjectUid(), false);
        getWorkingPatch()->addNewElement(inst->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, inst, inst->getObjectUid(), 1);

    }
}

const QVector<TableInstance*>& DefaultVersionImplementation::getTableInstances() const
{
    return m_data.m_tableInstances;
}

UserDataType* DefaultVersionImplementation::provideDatatypeForSqlType(const QString& name, const QString& sql, const QString& nullable, const QString& defaultValue, bool relaxed)
{
    QString type = sql;
    QString size = "";
    QString finalName = relaxed?
                        type + (size.length()>0?" ("+ size+")":"") + (defaultValue.length() > 0? ("=" + defaultValue) : "") + (nullable=="NO"?" Not Null":""):
                        name + (defaultValue.length() > 0? (" (Def: " + defaultValue) + ")" : "") + (nullable=="NO"?" Not Null":"");
    int stp = sql.indexOf('(') ;
    if(stp != -1)
    {
        type = sql.left(stp);
        size = sql.mid(stp + 1, sql.indexOf(')') - stp -1);
    }

    for(int i=0; i<m_data.m_dataTypes.size(); i++)
    {
        UserDataType* udt = m_data.m_dataTypes.at(i);
        if(QString::compare(udt->getSqlType(), type, Qt::CaseInsensitive) == 0)
        {
            if(udt->getSize() == size)
            {
                if((udt->isNullable() && QString::compare(nullable, "YES", Qt::CaseInsensitive) == 0) || (!udt->isNullable() && QString::compare(nullable, "NO", Qt::CaseInsensitive) == 0))
                {
                    if(udt->getDefaultValue() == defaultValue)
                    {
                        if(!relaxed)
                        {
                            if(udt->getName() == finalName)
                            {
                                return udt;
                            }
                        }
                        else
                        {
                            return udt;
                        }
                    }
                }
            }
        }
    }

    // nothing found, we should create a new data type with some default values
    UserDataType* newUdt = new UserDataType(finalName,
                                            Workspace::getInstance()->currentProjectsEngine()->getTypeStringForSqlType(type),
                                            type, size, defaultValue, QStringList(), false, type + " " + size,
                                            QString::compare(nullable, "YES", Qt::CaseInsensitive) == 0, QUuid::createUuid().toString(), this);

    newUdt->setName(NameGenerator::getUniqueName(this, (itemGetter)&Version::getDataType, newUdt->getName()));
    addNewDataType(newUdt, true);
    return newUdt;
}

QVector<Issue*> DefaultVersionImplementation::checkIssuesOfNewColumn(Column* inNewColumn, Table* inTable)
{
    QVector<Issue*> result;
    // firstly we need to check that the newly introduced columns' datatype is used in any other table
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        Table* tabI = m_data.m_tables.at(i);
        if(tabI != inTable)
        {
            QStringList fullColumns = tabI->fullColumns();
            for(int j=0; j<fullColumns.size(); j++)
            {
                const UserDataType* othersDataType = tabI->getDataTypeOfColumn(fullColumns.at(j));
                if(inNewColumn->getDataType()->getName() == othersDataType->getName())
                {
                    Column* otherColumn = tabI->getColumn(fullColumns.at(j));
                    if(otherColumn == 0) otherColumn = tabI->getColumnFromParents(fullColumns.at(j));
                    if(otherColumn == 0) continue;

                    // they share the same data type ... let's check if this data type is:
                    // 1. numeric and 2. primary key in one of them
                    if(othersDataType->getType() == DT_NUMERIC)
                    {
                        if(inNewColumn->isPk() || otherColumn->isPk()) // but ONLY if there is no foreign key between these two. TODO: check this too
                        {
                            if(m_validationFlags != 1)
                            {
                                Issue* issue = IssueManager::getInstance().createForeignKeyReccomendedIssue(tabI, otherColumn, inTable, inNewColumn);
                                if(issue)
                                {
                                    result.append(issue);
                                }
                            }
                        }
                    }
                    else
                    {
                        if(othersDataType->getType() != DT_DATETIME) // normalization does not matter for date/time
                        {
                            if(othersDataType->getSize().toInt() > 1)   // and for texts with at least 2 characters
                            {
                                Issue* issue = IssueManager::getInstance().createDatabaseNormalizationIssue(tabI, otherColumn, inTable, inNewColumn);
                                if(issue)
                                {
                                    result.append(issue);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

void DefaultVersionImplementation::addIssuse(Issue* issue)
{
    m_data.m_issues.append(issue);
}

Issue* DefaultVersionImplementation::getIssue(const QString& name)
{
    for(int i=0; i< m_data.m_issues.size(); i++)
    {
        if(m_data.m_issues[i]->getName() == name)
        {
            return m_data.m_issues[i];
        }
    }
    return 0;
}

void DefaultVersionImplementation::removeIssue(const QString& name)
{
    for(int i=0; i< m_data.m_issues.size(); i++)
    {
        if(m_data.m_issues[i]->getName() == name)
        {
            m_data.m_issues.remove(i);
            return;
        }
    }
}

QVector<Issue*>& DefaultVersionImplementation::getIssues()
{
    return m_data.m_issues;
}

void DefaultVersionImplementation::validateVersion(bool onRequest)
{
    if(!onRequest && !Configuration::instance().continuousValidation()) return;
    QVector<Issue*>& allIssues = getIssues();
    int i=0;
    while(i<allIssues.size())
    {
        if(!allIssues.at(i)->stillValid())
        {
            QString issueIName = allIssues.at(i)->getName();
            IssueOriginator* orig = allIssues.at(i)->getOriginator();
            removeIssue(issueIName);
            orig->removeIssueByName(issueIName);
            getGui()->cleanupOrphanedIssueTableItems();
        }
        else
        {
            i++;
        }
    }

    for(int tab_ctr = 0; tab_ctr<m_data.m_tables.size(); tab_ctr ++)
    {
        for(int i=0; i<m_data.m_tables.at(tab_ctr)->fullColumns().size(); i++)
        {
            Column* c = m_data.m_tables.at(tab_ctr)->getColumn(m_data.m_tables.at(tab_ctr)->fullColumns().at(i));
            if(c == 0) c = m_data.m_tables.at(tab_ctr)->getColumnFromParents(m_data.m_tables.at(tab_ctr)->fullColumns().at(i));
            checkIssuesOfNewColumn(c, m_data.m_tables.at(tab_ctr));
        }
    }
}

void DefaultVersionImplementation::setSpecialValidationFlags(int a)
{
    m_validationFlags = a;
}

View* DefaultVersionImplementation::getView(const QString &viewName) const
{
    for(int i=0; i< m_data.m_views.size(); i++)
    {
        if(m_data.m_views[i]->getName() == viewName)
        {
            return m_data.m_views[i];
        }
    }
    return 0;
}

void DefaultVersionImplementation::addView(View* v, bool initial)
{
    m_data.m_views.append(v);

    if(lockState() == LockableElement::LOCKED && !initial)
    {
        MainWindow::instance()->createPatchElement(this, v, v->getObjectUid(), false);
        getWorkingPatch()->addNewElement(v->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, v, v->getObjectUid(), 1);
    }

}

Trigger* DefaultVersionImplementation::getTrigger(const QString &triggerName) const
{
    for(int i=0; i< m_data.m_triggers.size(); i++)
    {
        if(m_data.m_triggers[i]->getName() == triggerName)
        {
            return m_data.m_triggers[i];
        }
    }
    return 0;
}

Function* DefaultVersionImplementation::getFunction(const QString &name) const
{
    for(int i=0; i< m_data.m_functions.size(); i++)
    {
        if(m_data.m_functions[i]->getName() == name)
        {
            return m_data.m_functions[i];
        }
    }
    return 0;
}

Procedure* DefaultVersionImplementation::getProcedure(const QString &procedureName) const
{
    for(int i=0; i< m_data.m_procedures.size(); i++)
    {
        if(m_data.m_procedures[i]->getName() == procedureName)
        {
            return m_data.m_procedures[i];
        }
    }
    return 0;
}

void DefaultVersionImplementation::addProcedure(Procedure* p, bool initial)
{
    m_data.m_procedures.append(p);

    if(lockState() == LockableElement::LOCKED && !initial)
    {
        MainWindow::instance()->createPatchElement(this, p, p->getObjectUid(), false);
        getWorkingPatch()->addNewElement(p->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, p, p->getObjectUid(), 1);
    }

}

void DefaultVersionImplementation::addFunction(Function* p, bool initial)
{
    m_data.m_functions.append(p);
    if(lockState() == LockableElement::LOCKED && !initial)
    {
        MainWindow::instance()->createPatchElement(this, p, p->getObjectUid(), false);
        getWorkingPatch()->addNewElement(p->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, p, p->getObjectUid(), 1);
    }
}

void DefaultVersionImplementation::addTrigger(Trigger* t, bool initial)
{
    m_data.m_triggers.append(t);
    if(lockState() == LockableElement::LOCKED && !initial)
    {
        MainWindow::instance()->createPatchElement(this, t, t->getObjectUid(), false);
        getWorkingPatch()->addNewElement(t->getObjectUid()); // this will be a new element ...
        MainWindow::instance()->updatePatchElementToReflectState(this, t, t->getObjectUid(), 1);
    }
}

const QVector<Procedure*>& DefaultVersionImplementation::getProcedures()
{
    return m_data.m_procedures;
}

const QVector<Function*>& DefaultVersionImplementation::getFunctions()
{
    return m_data.m_functions;
}

const QVector<Trigger*>& DefaultVersionImplementation::getTriggers()
{
    return m_data.m_triggers;
}

void DefaultVersionImplementation::cleanupDataTypes()
{
    QVector<int> sentencedIndices;
    for(int k=0; k<m_data.m_dataTypes.size(); k++)
    {
        QString dtName = m_data.m_dataTypes.at(k)->getName();
        const QVector<Table*>& allTables = getTables();
        QString usage = "";
        for(int i=0; i<allTables.size(); i++)
        {
            const QVector<Column*> & tablesColumns = allTables.at(i)->getColumns();
            for(int j=0; j<tablesColumns.size(); j++)
            {
                QString cn = tablesColumns.at(j)->getDataType()->getName();
                if(cn == dtName)
                {
                    usage += "Y";
                }
            }
        }

        if(usage.length() == 0)
        {
            sentencedIndices.prepend(k);
        }
    }

    for(int i=0; i<sentencedIndices.size(); i++)
    {
        QString dtName = m_data.m_dataTypes.at(sentencedIndices.at(i))->getName();
        delete m_data.m_dataTypes.at(sentencedIndices.at(i))->getLocation();
        deleteDataType(dtName);
    }

    getGui()->cleanupDtEntries();
}

VersionGuiElements* DefaultVersionImplementation::getGui()
{
    return m_guiElements;
}

QString DefaultVersionImplementation::getVersionText()
{
    return version;
}

SqlSource *DefaultVersionImplementation::getSqlSourceEntityWithGuid(const QString &guid) const
{
    Table* table =  dynamic_cast<Table*>(UidWarehouse::instance().getElement(guid));
    if(table) return table;

    if(m_project->oopProject())
    {
        TableInstance* tinst =  dynamic_cast<TableInstance*>(UidWarehouse::instance().getElement(guid));
        if(tinst) return tinst;
    }

    Procedure* p = dynamic_cast<Procedure*>(UidWarehouse::instance().getElement(guid));
    if(p) return p;

    Function* f = dynamic_cast<Function*>(UidWarehouse::instance().getElement(guid));
    if(f) return f;

    Trigger* t = dynamic_cast<Trigger*>(UidWarehouse::instance().getElement(guid));
    if(t) return t;

    View* v = dynamic_cast<View*>(UidWarehouse::instance().getElement(guid));
    if(v) return v;

    return 0;
}

bool DefaultVersionImplementation::cloneInto(Version* other, LockType lt)
{
    lock(lt);

    // clone the data types
    const QVector<UserDataType*> dts = getDataTypes();
    for(int i=0; i<dts.size(); i++)
    {
        other->addNewDataType(dynamic_cast<UserDataType*>(dts.at(i)->clone(this, other)), true);
        // lock the datatype
        dts.at(i)->lock(lt);
        dts.at(i)->updateGui();
    }

    // clone the tables
    const QVector<Table*> tabs = getTables();
    for(int i=0; i<tabs.size(); i++)
    {
        Table* newTable = dynamic_cast<Table*>(tabs.at(i)->clone(this, other));
        newTable->setName(tabs.at(i)->getName());
        other->addTable(newTable, true);
        // lock the table
        tabs.at(i)->lock(lt);
        tabs.at(i)->updateGui();
    }

    // now fix the foreign keys of the table
    for(int i=0; i<tabs.size(); i++)
    {
        const QVector<ForeignKey*>& ofks = tabs.at(i)->getForeignKeys();
        for(int j=0; j<ofks.size(); j++)
        {
            ForeignKey* fkj = ofks.at(j);
            ForeignKey* newfk = new ForeignKey(other, QUuid::createUuid().toString());
            newfk->setName(fkj->getName());
            newfk->setSourceUid(fkj->getObjectUid());
            const QVector<ForeignKey::ColumnAssociation*>& ofkAssocs = fkj->getAssociations();
            for(int k=0; k<ofkAssocs.size(); k++)
            {
                QString otherSForeginTable = ofkAssocs.at(k)->getSForeignTable();
                QString otherSLocalTable = ofkAssocs.at(k)->getSLocalTable();
                Table* foreignTable = other->getTable(otherSForeginTable);
                Table* localTable = other->getTable(otherSLocalTable);
                Column* foreignCol = foreignTable->getColumn(ofkAssocs.at(k)->getSForeignColumn());
                Column* localCol = localTable->getColumn(ofkAssocs.at(k)->getSLocalColumn());
                newfk->addAssociation(new ForeignKey::ColumnAssociation(foreignTable, foreignCol, localTable, localCol));
                newfk->setOnDelete(fkj->getOnDelete());
                newfk->setOnUpdate(fkj->getOnUpdate());
            }

            const QVector<Index*>& autoGenIndices = fkj->getAutoGenIndices();
            for(int k=0; k<autoGenIndices.size(); k++)
            {
                newfk->addAutogeneratedIndex(dynamic_cast<Index*>(autoGenIndices.at(k)->clone(this, other)));
            }
            other->getTable(tabs.at(i)->getName())->addForeignKey(newfk);
        }
    }

    // fix the autogenerated indices for the foreign keys
    for(int i=0; i<other->getTables().size(); i++)
    {
        Table* tabI = other->getTables().at(i);
        for(int j=0; j<tabI->getForeignKeys().size(); j++)
        {
            ForeignKey* fkJ = tabI->getForeignKeys()[j];
            fkJ->finalizeAutogeneratedIndices(other);
        }
    }

    // now fix the parent/child relationships of the tables
    for(int i=0; i<tabs.size(); i++)
    {
        Table* origparent = tabs.at(i)->getParent();
        if(origparent)
        {
            QString pname = origparent->getName();
            Table* this_parent = other->getTable(pname);    // name should be the same
            Table* this_table= other->getTable(tabs.at(i)->getName());
            this_table->setParent(this_parent);
            this_table->setParentUid(this_parent->getObjectUid());
        }
    }

    // clone the table instances
    QVector<TableInstance*> tinsts = getTableInstances();
    for(int i=0; i<tinsts.size(); i++)
    {
        TableInstance* newTinst = dynamic_cast<TableInstance*>(tinsts.at(i)->clone(this, other));

        // this also fixes the tables with the required link
        newTinst->finalizeCloning(tinsts.at(i), this, other);
        other->addTableInstance(newTinst, true);
        tinsts.at(i)->lock(lt);
        tinsts.at(i)->updateGui();
    }

    // fix the automatically instantiated table instances for the table instances
    QVector<TableInstance*> otherTinsts = other->getTableInstances();
    for(int i=0; i<otherTinsts.size(); i++)
    {
        TableInstance* srcTinst = getTableInstance(otherTinsts.at(i)->getName());
        otherTinsts.at(i)->finalizeAutoinstantiatedTinsts(srcTinst, this, other);
    }

    // clone the views
    const QVector<View*> views = getViews();
    for(int i=0; i<views.size(); i++)
    {
        View* v =  dynamic_cast<View*>(views.at(i)->clone(this, other));
        views.at(i)->lock(lt);
        views.at(i)->updateGui();
        other->addView(v, true);
    }

    // clone the procedures
    const QVector<Procedure*> procs = getProcedures();
    for(int i=0; i<procs.size(); i++)
    {
        Procedure* newp = dynamic_cast<Procedure*>(procs.at(i)->clone(this, other));
        other->addProcedure(newp, true);
        procs.at(i)->lock(lt);
        procs.at(i)->updateGui();
    }

    // and the functions
    const QVector<Function*> funcs = getFunctions();
    for(int i=0; i<funcs.size(); i++)
    {
        Function* newp = dynamic_cast<Function*>(funcs.at(i)->clone(this, other));
        other->addFunction(newp, true);
        funcs.at(i)->lock(lt);
        funcs.at(i)->updateGui();
    }

    // clone the triggers
    const QVector<Trigger*> trigs = getTriggers();
    for(int i=0; i<trigs.size(); i++)
    {
        Trigger* newp = dynamic_cast<Trigger*>(trigs.at(i)->clone(this, other));
        other->addTrigger(newp, true);
        trigs.at(i)->lock(lt);
        trigs.at(i)->updateGui();
    }

    // clone the diagrams
    const QVector<Diagram*> dias = getDiagrams();
    for(int i=0; i<dias.size(); i++)
    {
        Diagram* dgr = dynamic_cast<Diagram*>(dias.at(i)->clone(this, other));
        other->addDiagram(dgr, true);
        dias.at(i)->lock(lt);
        dias.at(i)->updateGui();
    }
    return true;
}

void DefaultVersionImplementation::patchItem(const QString &/*uid*/)
{

}

CloneableElement* DefaultVersionImplementation::clone(Version* /*sourceVersion*/, Version* /*targetVersion*/)
{
    return 0;
}

void DefaultVersionImplementation::replaceTable(const QString& uid, Table* newTab)
{
    Table* t = getTableWithUid(uid);
    if(!t) return;

    newTab->setForcedUid(uid);
    newTab->setName(t->getName());
    newTab->lock(LockableElement::LOCKED);
    newTab->setLocation(t->getLocation());

    for(int i=0; i< m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getObjectUid() == uid)
        {
            m_data.m_tables[i] = newTab;
        }
    }

    newTab->updateGui();

    return;
}

void DefaultVersionImplementation::replaceTableInstance(const QString& uid, TableInstance* newInst)
{
    TableInstance* t = getTableInstanceWithUid(uid);
    if(!t) return;

    newInst->setForcedUid(uid);
    newInst->setName(t->getName());
    newInst->lock(LockableElement::LOCKED);
    newInst->setLocation(t->getLocation());

    for(int i=0; i< m_data.m_tableInstances.size(); i++)
    {
        if(m_data.m_tableInstances[i]->getObjectUid() == uid)
        {
            m_data.m_tableInstances[i] = newInst;
        }
    }

    newInst->updateGui();

    return;
}


void DefaultVersionImplementation::replaceDiagram(const QString& uid, Diagram* withDgram)
{
    Diagram* d = getDiagramWithUid(uid);
    if(!d) return;

    withDgram->setForcedUid(uid);
    withDgram->setName(d->getName());
    withDgram->lock(LockableElement::LOCKED);
    withDgram->setLocation(d->getLocation());

    for(int i=0; i< m_data.m_diagrams.size(); i++)
    {
        if(m_data.m_diagrams[i]->getObjectUid() == uid)
        {
            m_data.m_diagrams[i] = withDgram;
        }
    }

    withDgram->updateGui();

    return;
}

void DefaultVersionImplementation::replaceView(const QString& uid, View* view)
{
    View* v = getViewWithUid(uid);
    if(!v) return;

    view->setForcedUid(uid);
    view->setName(v->getName());
    view->lock(LockableElement::LOCKED);
    view->setLocation(v->getLocation());

    for(int i=0; i< m_data.m_views.size(); i++)
    {
        if(m_data.m_views[i]->getObjectUid() == uid)
        {
            m_data.m_views[i] = view;
        }
    }

    view->updateGui();

    return;
}

void DefaultVersionImplementation::replaceProcedure(const QString& uid, Procedure* proc)
{
    Procedure* p = getProcedureWithUid(uid);
    if(!p) return;

    proc->setForcedUid(uid);
    proc->setName(p->getName());
    proc->lock(LockableElement::LOCKED);
    proc->setLocation(p->getLocation());

    for(int i=0; i< m_data.m_procedures.size(); i++)
    {
        if(m_data.m_procedures[i]->getObjectUid() == uid)
        {
            m_data.m_procedures[i] = proc;
        }
    }
    proc->updateGui();
    return;
}

void DefaultVersionImplementation::replaceFunction(const QString& uid, Function* func)
{
    Function* f = getFunctionWithUid(uid);
    if(!f) return;

    func->setForcedUid(uid);
    func->setName(f->getName());
    func->lock(LockableElement::LOCKED);
    func->setLocation(f->getLocation());

    for(int i=0; i< m_data.m_functions.size(); i++)
    {
        if(m_data.m_functions[i]->getObjectUid() == uid)
        {
            m_data.m_functions[i] = func;
        }
    }

    func->updateGui();
    return;
}

void DefaultVersionImplementation::replaceUserDataType(const QString& uid, UserDataType* dt)
{
    UserDataType* d = getUserDataTypeWithUid(uid);
    if(!d) return;

    dt->setForcedUid(uid);
    dt->setName(d->getName());
    dt->lock(LockableElement::LOCKED);
    dt->setLocation(d->getLocation());

    for(int i=0; i< m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes[i]->getObjectUid() == uid)
        {
            m_data.m_dataTypes[i] = dt;
        }
    }
    dt->updateGui();
    return;
}

void DefaultVersionImplementation::replaceTrigger(const QString& uid, Trigger* trg)
{
    Trigger* t = getTriggerWithUid(uid);
    if(!t) return;

    trg->setForcedUid(uid);
    trg->setName(t->getName());
    trg->lock(LockableElement::LOCKED);
    trg->setLocation(t->getLocation());

    for(int i=0; i< m_data.m_triggers.size(); i++)
    {
        if(m_data.m_triggers[i]->getObjectUid() == uid)
        {
            m_data.m_triggers[i] = trg;
        }
    }

    trg->updateGui();

    return;
}


void DefaultVersionImplementation::addPatch(Patch* p)
{
    m_patches.append(p);
    if(!p->isSuspended()) m_currentPatchIndex = m_patches.indexOf(p);
}

Patch* DefaultVersionImplementation::getWorkingPatch()
{
    m_currentPatchIndex = -1;

    if(m_patches.size() == 0)
    {
        Patch* p = new Patch(this, false);
        m_patches.append(p);
    }
    for(int i=0; i<m_patches.size(); i++)
    {
        if(!m_patches.at(i)->isSuspended())
        {
            m_currentPatchIndex = i;
        }
    }

    if(m_currentPatchIndex == -1) // all the patches are suspended
    {
        Patch* p = new Patch(this, false);
        m_patches.append(p);
        m_currentPatchIndex = 0;
    }
    return m_patches.at(m_currentPatchIndex);
}

DefaultVersionImplementation::CAN_UNDELETE_STATUS DefaultVersionImplementation::canUndeleteTable(const QString &uid, QString &extra)
{
    ObjectWithUid* obj = getWorkingPatch()->getDeletedObject(uid);
    if(!obj)
    {
        extra = "Deleted object not found in patch.";
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    TableDeletionAction* tda = getWorkingPatch()->getTDA(uid);
    if(!tda)
    {
        extra = "Deletion action not found in patch.";
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    for(int i=0; i<tda->deletedTableInstances.size(); i++)
    {
        Table* instancesTable = tda->deletedTableInstances.at(i)->table();
        if(instancesTable)
        {
            if(!hasTable(instancesTable) && instancesTable->getObjectUid() != tda->deletedTable->getObjectUid())
            {
                extra = tda->deletedTableInstances.at(i)->table()->getName();
                return DEPENDENT_TABLE_WAS_NOT_FOUND_IN_VERSION;
            }
        }
        else
        {
            extra = "Tinst: " + tda->deletedTableInstances.at(i)->getName() + " <- Table for this in version not found. ";
            return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
        }
    }
    return CAN_UNDELETE;
}

DefaultVersionImplementation::CAN_UNDELETE_STATUS DefaultVersionImplementation::canUndeleteDiagram(const QString &uid, QString &extra)
{
    ObjectWithUid* obj = getWorkingPatch()->getDeletedObject(uid);
    if(!obj)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    DiagramDeletionAction* dda = getWorkingPatch()->getDDA(uid);
    if(!dda)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    Diagram* dgr = dda->deletedDiagram;
    QStringList tabsInDgr = dgr->getTableNames();
    for(int i=0; i<tabsInDgr.size(); i++)
    {
        if(!hasTable(tabsInDgr.at(i)))
        {
            extra = tabsInDgr.at(i);
            return DEPENDENT_TABLE_WAS_NOT_FOUND_IN_VERSION;
        }
    }
    return CAN_UNDELETE;
}

DefaultVersionImplementation::CAN_UNDELETE_STATUS DefaultVersionImplementation::canUndeleteProcedure(const QString &uid, QString &/*extra*/)
{
    ObjectWithUid* obj = getWorkingPatch()->getDeletedObject(uid);
    if(!obj)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    ProcedureDeletionAction* pda = getWorkingPatch()->getPDA(uid);
    if(!pda)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    return CAN_UNDELETE;
}

DefaultVersionImplementation::CAN_UNDELETE_STATUS DefaultVersionImplementation::canUndeleteFunction(const QString &uid, QString &/*extra*/)
{
    ObjectWithUid* obj = getWorkingPatch()->getDeletedObject(uid);
    if(!obj)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    FunctionDeletionAction* pda = getWorkingPatch()->getFDA(uid);
    if(!pda)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    return CAN_UNDELETE;
}

DefaultVersionImplementation::CAN_UNDELETE_STATUS DefaultVersionImplementation::canUndeleteTrigger(const QString &uid, QString &extra)
{
    ObjectWithUid* obj = getWorkingPatch()->getDeletedObject(uid);
    if(!obj)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    TriggerDeletionAction* tda = getWorkingPatch()->getTrDA(uid);
    if(!tda)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    if(!hasTable(tda->deletedTrigger->getTable()))
    {
        extra = tda->deletedTrigger->getTable();
        return DEPENDENT_TABLE_WAS_NOT_FOUND_IN_VERSION;
    }

    return CAN_UNDELETE;
}

DefaultVersionImplementation::CAN_UNDELETE_STATUS DefaultVersionImplementation::canUndeleteView(const QString &uid, QString& extra)
{
    ObjectWithUid* obj = getWorkingPatch()->getDeletedObject(uid);
    if(!obj)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    ViewDeletionAction* tda = getWorkingPatch()->getVDA(uid);
    if(!tda)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    QVector<const Table*> usedTabs = tda->deletedView->getSourceTables();
    for(int i=0; i<usedTabs.size(); i++)
    {
        if(!hasTable(usedTabs.at(i)->getName()))
        {
            extra = usedTabs.at(i)->getName();
            return DEPENDENT_TABLE_WAS_NOT_FOUND_IN_VERSION;
        }
    }

    return CAN_UNDELETE;
}

DefaultVersionImplementation::CAN_UNDELETE_STATUS DefaultVersionImplementation::canUndeleteDataType(const QString &uid, QString &/*extra*/)
{
    ObjectWithUid* obj = getWorkingPatch()->getDeletedObject(uid);
    if(!obj)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    DataTypeDeletionAction* dtda = getWorkingPatch()->getDtDA(uid);
    if(!dtda)
    {
        return DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH;
    }

    return CAN_UNDELETE;
}

Version::PatchTreeRemovalStatus DefaultVersionImplementation::undeleteObject(const QString& uid, bool suspend)
{
    QString extra;

    // can we undelete a table or a table instance?
    DefaultVersionImplementation::CAN_UNDELETE_STATUS canUndelete = canUndeleteTable(uid, extra);
    if(canUndelete != CAN_UNDELETE)
    {

        // can we undelete a diagram?
        canUndelete = canUndeleteDiagram(uid, extra);
        if(canUndelete == CAN_UNDELETE)
        {
            DiagramDeletionAction* dda = getWorkingPatch()->getDDA(uid);
            addDiagram(dda->deletedDiagram, true);
            getGui()->createDiagramTreeEntry(dda->deletedDiagram);
            dda->deletedDiagram->updateGui();
            bool x = getWorkingPatch()->undeleteObject(uid);
            if(x)
                return REMOVE_FROM_PATCH_TREE;
            else
                return DO_NOT_REMOVE_FROM_PATCH_TREE;
        }

        // see if we can undelete a procedure?
        canUndelete = canUndeleteProcedure(uid, extra);
        if(canUndelete == CAN_UNDELETE)
        {
            ProcedureDeletionAction* pda = getWorkingPatch()->getPDA(uid);
            addProcedure(pda->deletedProcedure, true);
            getGui()->createProcedureTreeEntry(pda->deletedProcedure);
            pda->deletedProcedure->updateGui();
            bool x = getWorkingPatch()->undeleteObject(uid);
            if(x)
                return REMOVE_FROM_PATCH_TREE;
            else
                return DO_NOT_REMOVE_FROM_PATCH_TREE;
        }

        // see if we can undelete a function?
        canUndelete = canUndeleteFunction(uid, extra);
        if(canUndelete == CAN_UNDELETE)
        {
            FunctionDeletionAction* pda = getWorkingPatch()->getFDA(uid);
            addFunction(pda->deletedFunction, true);
            getGui()->createFunctionTreeEntry(pda->deletedFunction);
            pda->deletedFunction->updateGui();
            bool x = getWorkingPatch()->undeleteObject(uid);
            if(x)
                return REMOVE_FROM_PATCH_TREE;
            else
                return DO_NOT_REMOVE_FROM_PATCH_TREE;
        }

        // see if we can undelete a trigger?
        canUndelete = canUndeleteTrigger(uid, extra);
        if(canUndelete == CAN_UNDELETE)
        {
            TriggerDeletionAction* tda = getWorkingPatch()->getTrDA(uid);
            addTrigger(tda->deletedTrigger, true);
            getGui()->createTriggerTreeEntry(tda->deletedTrigger);
            tda->deletedTrigger->updateGui();
            bool x = getWorkingPatch()->undeleteObject(uid);
            if(x)
                return REMOVE_FROM_PATCH_TREE;
            else
                return DO_NOT_REMOVE_FROM_PATCH_TREE;
        }

        // see if we can undelete a trigger?
        canUndelete = canUndeleteView(uid, extra);
        if(canUndelete == CAN_UNDELETE)
        {
            ViewDeletionAction* vda = getWorkingPatch()->getVDA(uid);
            addView(vda->deletedView, true);
            getGui()->createViewTreeEntry(vda->deletedView);
            vda->deletedView->updateGui();
            bool x = getWorkingPatch()->undeleteObject(uid);
            if(x)
                return REMOVE_FROM_PATCH_TREE;
            else
                return DO_NOT_REMOVE_FROM_PATCH_TREE;
        }

        // see if we can undelete a data type?
        canUndelete = canUndeleteDataType(uid, extra);
        if(canUndelete == CAN_UNDELETE)
        {
            DataTypeDeletionAction* dtda = getWorkingPatch()->getDtDA(uid);
            addNewDataType(dtda->deletedDataType, true);
            getGui()->createDataTypeTreeEntry(dtda->deletedDataType);
            dtda->deletedDataType->updateGui();
            bool x = getWorkingPatch()->undeleteObject(uid);
            if(x)
                return REMOVE_FROM_PATCH_TREE;
            else
                return DO_NOT_REMOVE_FROM_PATCH_TREE;
        }

        // hmm, cannot undelete
        if(canUndelete == DEPENDENT_TABLE_WAS_NOT_FOUND_IN_VERSION && !suspend)
        {
//            qDebug() << "UID(1) " << uid;
            QMessageBox::critical(MainWindow::instance(), QObject::tr("Error"), QObject::tr("Cannot undelete this object: ") + extra + QObject::tr(" missing from version."), QMessageBox::Ok);
            return DO_NOT_REMOVE_FROM_PATCH_TREE_FAILURE;
        }

        if(!suspend)
        {
//            qDebug() << "UID(2) " << uid;
            QMessageBox::critical(MainWindow::instance(), QObject::tr("Error"), QObject::tr("Cannot undelete this object: ") + extra + QObject::tr(" missing from version."), QMessageBox::Ok);
            return DO_NOT_REMOVE_FROM_PATCH_TREE_FAILURE;
        }

    }
    else // undelete a table
    {
        // undelete
        TableDeletionAction* tda = getWorkingPatch()->getTDA(uid);
        if(tda->deletedTable) // obivously a table was deleted
        {
            addTable(tda->deletedTable, true);
            tda->deletedTable->lock(LOCKED);
            // now see if this had a parent table or not
//            qDebug() << "create tbl " << uid << " obj=" << tda->deletedTable;
            if(tda->parentTable)
            {
                const_cast<Table*>(tda->parentTable)->addSpecializedTable(tda->deletedTable);
                getGui()->createTableTreeEntry(tda->deletedTable, tda->parentTable->getLocation());
            }
            else
            {
                getGui()->createTableTreeEntry(tda->deletedTable, getGui()->getTablesItem());
            }
            // and the table instances
            for(int i=0; i<tda->deletedTableInstances.size(); i++)
            {
                addTableInstance(tda->deletedTableInstances.at(i), true);

                getGui()->createTableInstanceTreeEntry(tda->deletedTableInstances.at(i));
                tda->deletedTableInstances.at(i)->unSentence();
                tda->deletedTableInstances.at(i)->undeleteObject();
                tda->deletedTableInstances.at(i)->updateGui();

                getWorkingPatch()->undeleteObject(tda->deletedTableInstances.at(i)->getObjectUid());
            }

            tda->deletedTable->updateGui();
        }
        else // just a table instance was deleted
        {
            // and the table instances
            for(int i=0; i<tda->deletedTableInstances.size(); i++)
            {
                addTableInstance(tda->deletedTableInstances.at(i), true);
                getGui()->createTableInstanceTreeEntry(tda->deletedTableInstances.at(i));
                tda->deletedTableInstances.at(i)->unSentence();
                tda->deletedTableInstances.at(i)->undeleteObject();
                tda->deletedTableInstances.at(i)->updateGui();
                getWorkingPatch()->undeleteObject(tda->deletedTableInstances.at(i)->getObjectUid());
            }
        }
    }

    // remove from the patch
    bool x = getWorkingPatch()->undeleteObject(uid);
    if(x)
        return REMOVE_FROM_PATCH_TREE;
    else
        return DO_NOT_REMOVE_FROM_PATCH_TREE;
 }

void DefaultVersionImplementation::removePatch(const Patch* p)
{
    if(m_patches.size() == 0) return;

    for(int i=0; i<m_patches.size(); i++)
    {
        if(m_patches.at(i)->getObjectUid() == p->getObjectUid())
        {
            m_currentPatchIndex = i - 1;
            m_patches.remove(i);
        }
    }
}

void DefaultVersionImplementation::lockVersion(LockableElement::LockType lt)
{
    lock(lt);
    updateGui();

    // clone the data types
    const QVector<UserDataType*> dts = getDataTypes();
    for(int i=0; i<dts.size(); i++)
    {
        // lock the datatype
        dts.at(i)->lock(lt);
        dts.at(i)->updateGui();
    }

    // clone the tables
    const QVector<Table*> tabs = getTables();
    for(int i=0; i<tabs.size(); i++)
    {
        // lock the table
        tabs.at(i)->lock(lt);
        tabs.at(i)->updateGui();
    }


    QVector<TableInstance*> tinsts = getTableInstances();
    for(int i=0; i<tinsts.size(); i++)
    {
        tinsts.at(i)->lock(lt);
        tinsts.at(i)->updateGui();
    }


    // clone the views
    const QVector<View*> views = getViews();
    for(int i=0; i<views.size(); i++)
    {
        views.at(i)->lock(lt);
        views.at(i)->updateGui();
    }

    // clone the procedures
    const QVector<Procedure*> procs = getProcedures();
    for(int i=0; i<procs.size(); i++)
    {
        procs.at(i)->lock(lt);
        procs.at(i)->updateGui();
    }

    // and the functions
    const QVector<Function*> funcs = getFunctions();
    for(int i=0; i<funcs.size(); i++)
    {
        funcs.at(i)->lock(lt);
        funcs.at(i)->updateGui();
    }

    // clone the triggers
    const QVector<Trigger*> trigs = getTriggers();
    for(int i=0; i<trigs.size(); i++)
    {
        trigs.at(i)->lock(lt);
        trigs.at(i)->updateGui();
    }

    // clone the diagrams
    const QVector<Diagram*> dias = getDiagrams();
    for(int i=0; i<dias.size(); i++)
    {
        dias.at(i)->lock(lt);
        dias.at(i)->updateGui();
    }
}

Table* DefaultVersionImplementation::getDescendantTable(Table* src)
{
    const QVector<Table*> &tables = getTables();
    for(int i=0; i<tables.size(); i++)
    {
        if(UidWarehouse::instance().related(tables[i], src))
        {
            return tables[i];
        }
    }
    return 0;
}

TableInstance* DefaultVersionImplementation::getDescendantTableInstance(TableInstance* src)
{
    const QVector<TableInstance*> &tables = getTableInstances();
    for(int i=0; i<tables.size(); i++)
    {
        if(UidWarehouse::instance().related(tables[i], src))
        {
            return tables[i];
        }
    }
    return 0;
}
