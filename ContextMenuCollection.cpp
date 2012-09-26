#include "ContextMenuCollection.h"
#include "IconFactory.h"
#include "core_ConnectionManager.h"
#include "uids.h"

ContextMenuCollection* ContextMenuCollection::m_instance = 0;

ContextMenuCollection::ContextMenuCollection()
{
    // popup menus
    m_tablePopupMenu = new QMenu();
    m_tableInstancePopupMenu = new QMenu();
    m_datatypePopupMenu = new QMenu();
    m_diagramPopupMenu = new QMenu();
    m_tablesPopupMenu = new QMenu();
    m_tableInstancesPopupMenu = new QMenu();
    m_diagramsPopupMenu = new QMenu();
    m_columnPopupMenu = new QMenu();
    m_datatypesPopupMenu = new QMenu();
    m_issuePopupMenu = new QMenu();
    m_issuesOfATablePopupMenu = new QMenu();
    m_createTableInstancesPopup = new QMenu();
    m_createNewViewPopupMenu = new QMenu();
    m_connectionPopupMenu = new QMenu();
    m_deployPopupMenu = new QMenu();
    m_browsedTablePopupMenu = new QMenu();
    m_viewsPopupMenu = new QMenu();
    m_viewPopupMenu = new QMenu();
    m_procedurePopupMenu = new QMenu();
    m_majorVersionPopupMenu = new QMenu();
    m_unlockTablePopupMenu = new QMenu();
    m_relockTablePopupMenu = new QMenu();
    m_unlockTriggerPopupMenu = new QMenu();
    m_relockTriggerPopupMenu = new QMenu();
    m_unlockTableInstancePopupMenu = new QMenu();
    m_relockTableInstancePopupMenu = new QMenu();
    m_unlockFunctionPopupMenu = new QMenu();
    m_relockFunctionPopupMenu = new QMenu();
    m_unlockProcedurePopupMenu = new QMenu();
    m_relockProcedurePopupMenu = new QMenu();
    m_unlockViewPopupMenu = new QMenu();
    m_relockViewPopupMenu = new QMenu();
    m_unlockDTPopupMenu = new QMenu();
    m_relockDTPopupMenu = new QMenu();
    m_unlockDiagramPopupMenu = new QMenu();
    m_relockDiagramPopupMenu = new QMenu();
    m_undeletePopupMenu = new QMenu();
    m_functionPopupMenu = new QMenu();
    m_triggerPopupMenu = new QMenu();
    m_suspendPatchMenu = new QMenu();
    m_resumePatchMenu = new QMenu();
    m_proceduresPopupMenu = new QMenu();
    m_functionsPopupMenu = new QMenu();
    m_triggersPopupMenu = new QMenu();

    // actions
    action_RemoveTable = new QAction(QObject::tr("Delete table"), 0);
    action_RemoveTable->setIcon(IconFactory::getRemoveIcon());
    action_DuplicateTable = new QAction(QObject::tr("Duplicate"), 0);
    action_TableAddColumn = new QAction(QObject::tr("Add column"), 0);
    action_SpecializeTable = new QAction(QObject::tr("Specialize"), 0);
    action_InstantiateTable = new QAction(QObject::tr("Instantiate"), 0);
    action_DeleteTableInstance = new QAction(QObject::tr("Delete"), 0);
    action_DeleteTableInstance->setIcon(IconFactory::getRemoveIcon());
    action_RenameTableInstance = new QAction(QObject::tr("Rename"), 0);
    action_DeleteDataType = new QAction(QObject::tr("Delete"), 0);
    action_DeleteDataType->setIcon(IconFactory::getRemoveIcon());
    action_DuplicateDataType = new QAction(QObject::tr("Duplicate"), 0);
    action_DeleteDiagram = new QAction(QObject::tr("Delete"), 0);
    action_DeleteDiagram->setIcon(IconFactory::getRemoveIcon());
    action_RenameDiagram = new QAction(QObject::tr("Rename Diagram"), 0);
    action_AddTable = new QAction(QObject::tr("New Table"), 0);
    action_CopyTable = new QAction(QObject::tr("Copy"), 0);
    action_PasteTable = new QAction(QObject::tr("Paste table"), 0);
    action_AddTableInstance = new QAction(QObject::tr("New Table Instance"), 0);
    action_AddDiagram = new QAction(QObject::tr("New Diagram"), 0);
    action_CopyColumn = new QAction(QObject::tr("Copy"), 0);
    action_PasteColumn = new QAction(QObject::tr("Paste"), 0);
    action_addString = new QAction(QObject::tr("New String Type"), 0);
    action_addNumeric = new QAction(QObject::tr("New Numeric Type"), 0);
    action_addDateType = new QAction(QObject::tr("New Date/Time Type"), 0);
    action_addBool = new QAction(QObject::tr("New Boolean Type"), 0);
    action_addMisc = new QAction(QObject::tr("New Misc Type"), 0);
    action_addBlob = new QAction(QObject::tr("New Blob Type"), 0);
    action_addSpatial = new QAction(QObject::tr("New Spatial Type"), 0);
    action_gotoIssueLocation = new QAction(QObject::tr("Goto Issue Origin"), 0);
    action_ignoreIssue = new QAction(QObject::tr("Ignore this issue"), 0);
    action_ignoreIssuesFromThisTable = new QAction(QObject::tr("Ignore issues"), 0);
    action_createViewUsingQueryBuilder = new QAction(QObject::tr("Using QueryBuilder"), 0);
    action_createViewUsingSql = new QAction(QObject::tr("SQL"), 0);
    action_connectionSqlQuery = new QAction(QObject::tr("Run Script"), 0);
    action_connectionSqlQuery->setIcon(IconFactory::getSqlIcon());
    action_connectionConnect = new QAction(QObject::tr("Connect"), 0);
    action_connectionConnect->setIcon(IconFactory::getConnectConnectionIcon());
    action_connectionDelete = new QAction(QObject::tr("Delete"), 0);
    action_connectionDelete->setIcon(IconFactory::getRemoveIcon());
    action_connectionEdit = new QAction(QObject::tr("Edit"), 0);
    action_connectionEdit->setIcon(IconFactory::getEditConnectionIcon());
    action_connectionBrowse = new QAction(QObject::tr("Browse"), 0);
    action_connectionBrowse->setIcon(IconFactory::getBrowseConnectionIcon());
    action_connectionDrop = new QAction(QObject::tr("Drop"), 0);
    action_connectionDrop->setIcon(IconFactory::getDropDatabaseIcon());
    action_connectionRecreate = new QAction(QObject::tr("Recreate"), 0);
    action_connectionRecreate->setIcon(IconFactory::getRecreateDatabaseIcon());
    action_browsedTableInjectIntoSolution = new QAction(QObject::tr("Add to current solution"), 0);
    action_browsedTableBrowse = new QAction(QObject::tr("Browse table"), 0);
    action_browsedTableView = new QAction(QObject::tr("View table details"), 0);
    action_deleteView = new QAction(QObject::tr("Delete view"), 0);
    action_deleteView->setIcon(IconFactory::getRemoveIcon());
    action_deleteTrigger = new QAction(QObject::tr("Delete trigger"), 0);
    action_deleteTrigger->setIcon(IconFactory::getRemoveIcon());
    action_deleteProcedure = new QAction(QObject::tr("Delete procedure"), 0);
    action_deleteProcedure->setIcon(IconFactory::getRemoveIcon());
    action_releaseMajorVersion = new QAction(QObject::tr("Release Version"), 0);
    action_releaseMajorVersion->setIcon(IconFactory::getReleaseMajorVersionIcon());
    action_unlock = new QAction(QObject::tr("Unlock"), 0);
    action_unlock->setIcon(IconFactory::getUnLockedIcon());
    action_relock = new QAction(QObject::tr("Lock"), 0);
    action_relock->setIcon(IconFactory::getLockedIcon());
    action_deleteFunction = new QAction(QObject::tr("Delete function"), 0);
    action_deleteFunction->setIcon(IconFactory::getRemoveIcon());
    action_suspendPatch = new QAction(QObject::tr("Suspend this patch"), 0);
    action_suspendPatch->setIcon(IconFactory::getSuspendPatchIcon());
    action_resumePatch = new QAction(QObject::tr("Resume this patch"), 0);
    action_renamePatch = new QAction(QObject::tr("Rename"), 0);
    action_undelete =  new QAction(QObject::tr("Undelete"), 0);
    action_undelete->setIcon(IconFactory::getUndeleteIcon());
    action_addProcedure =  new QAction(QObject::tr("New Procedure"), 0);
    action_addFunction =  new QAction(QObject::tr("New Function"), 0);
    action_addTrigger =  new QAction(QObject::tr("New Trigger"), 0);

    // populate the table popup menu
    m_tablePopupMenu->setTitle(QObject::tr("Table"));
    m_tablePopupMenu->setIcon(IconFactory::getTableIcon());
    m_tablePopupMenu->addAction(action_TableAddColumn);
    m_tablePopupMenu->addSeparator();
    m_tablePopupMenu->addAction(action_RemoveTable);
    m_tablePopupMenu->addAction(action_DuplicateTable);
    m_tablePopupMenu->addAction(action_SpecializeTable);
    m_tablePopupMenu->addSeparator();
    m_tablePopupMenu->addAction(action_InstantiateTable);
    m_tablePopupMenu->addSeparator();
    m_tablePopupMenu->addAction(action_CopyTable);

    // populate the table instances popup menu
    m_tableInstancePopupMenu->setTitle(QObject::tr("Table Instance"));
    m_tableInstancePopupMenu->setIcon(IconFactory::getTabinstIcon());
    m_tableInstancePopupMenu->addAction(action_DeleteTableInstance);
    m_tableInstancePopupMenu->addAction(action_RenameTableInstance);

    // datatypes popup menu
    m_datatypePopupMenu->addAction(action_DeleteDataType);
    m_datatypePopupMenu->addAction(action_DuplicateDataType);

    // diagrams popup menu
    m_diagramPopupMenu->setTitle(QObject::tr("Diagram"));
    m_diagramPopupMenu->setIcon(IconFactory::getDiagramIcon());
    m_diagramPopupMenu->addAction(action_DeleteDiagram);
    m_diagramPopupMenu->addAction(action_RenameDiagram);

    //tables popup menu
    m_tablesPopupMenu->addAction(action_AddTable);
    m_tablesPopupMenu->addAction(action_PasteTable);

    // table instances
    m_tableInstancesPopupMenu->addAction(action_AddTableInstance);

    // diagrams
    m_diagramsPopupMenu->addAction(action_AddDiagram);

    // procedures
    m_proceduresPopupMenu->addAction(action_addProcedure);

    // triggers
    m_triggersPopupMenu->addAction(action_addTrigger);

    // functions
    m_functionsPopupMenu->addAction(action_addFunction);

    // columns
    m_columnPopupMenu->addAction(action_CopyColumn);
    m_columnPopupMenu->addAction(action_PasteColumn);

    // data types
    m_datatypesPopupMenu->addAction(action_addString);
    m_datatypesPopupMenu->addAction(action_addNumeric);
    m_datatypesPopupMenu->addAction(action_addBool);
    m_datatypesPopupMenu->addAction(action_addDateType);
    m_datatypesPopupMenu->addAction(action_addBlob);
    m_datatypesPopupMenu->addAction(action_addMisc);
    m_datatypesPopupMenu->addAction(action_addSpatial);

    // new view popup
    m_createNewViewPopupMenu->addAction(action_createViewUsingQueryBuilder);
    m_createNewViewPopupMenu->addAction(action_createViewUsingSql);

    // issues
    m_issuePopupMenu->addAction(action_gotoIssueLocation);
    m_issuePopupMenu->addAction(action_ignoreIssue);

    // issues of a table
    m_issuesOfATablePopupMenu->addAction(action_ignoreIssuesFromThisTable);

    // connections
    m_connectionPopupMenu->addAction(action_connectionSqlQuery);
    m_connectionPopupMenu->addAction(action_connectionConnect);
    m_connectionPopupMenu->addAction(action_connectionBrowse);
    m_connectionPopupMenu->addAction(action_connectionEdit);
    m_connectionPopupMenu->addAction(action_connectionRecreate);
    m_connectionPopupMenu->addAction(action_connectionDelete);
    m_connectionPopupMenu->addAction(action_connectionDrop);

    // browsed table
    m_browsedTablePopupMenu->addAction(action_browsedTableInjectIntoSolution);
    m_browsedTablePopupMenu->addAction(action_browsedTableBrowse);
    //m_browsedTablePopupMenu->addAction(action_browsedTableView);
    action_browsedTableInjectIntoSolution->setVisible(false);
    action_browsedTableView->setEnabled(false);

    // popup menu for the table instances
    m_createTableInstancesPopup->clear();

    // one views popup menu
    m_viewPopupMenu->addAction(action_deleteView);

    // one procedures popup menu
    m_procedurePopupMenu->setTitle(QObject::tr("Procedure"));
    m_procedurePopupMenu->setIcon(IconFactory::getProcedureIcon());
    m_procedurePopupMenu->addAction(action_deleteProcedure);

    // the major version popup menu
    m_majorVersionPopupMenu->addAction(action_releaseMajorVersion);

    // one functions popup menu
    m_functionPopupMenu->addAction(action_deleteFunction);
    m_functionPopupMenu->setTitle(QObject::tr("Function"));
    m_functionPopupMenu->setIcon(IconFactory::getFunctionTreeIcon());

    // one triggers' menu
    m_triggerPopupMenu->setTitle(QObject::tr("Trigger"));
    m_triggerPopupMenu->setIcon(IconFactory::getTriggerIcon());
    m_triggerPopupMenu->addAction(action_deleteTrigger);

    // the patch's menu
    m_suspendPatchMenu->addAction(action_suspendPatch);
    m_suspendPatchMenu->addAction(action_renamePatch);

    // the locked table popup menus
    m_unlockTablePopupMenu->addAction(action_unlock);
    m_relockTablePopupMenu->addAction(action_relock);
    m_unlockTablePopupMenu->addAction(action_RemoveTable);
    m_relockTablePopupMenu->addMenu(m_tablePopupMenu);

    // the locked trigger popup menus
    m_unlockTriggerPopupMenu->addAction(action_unlock);
    m_relockTriggerPopupMenu->addAction(action_relock);
    m_unlockTriggerPopupMenu->addMenu(m_triggerPopupMenu);
    m_relockTriggerPopupMenu->addMenu(m_triggerPopupMenu);

    // the locked tab inst popup menus
    m_unlockTableInstancePopupMenu->addAction(action_unlock);
    m_relockTableInstancePopupMenu->addAction(action_relock);
    m_unlockTableInstancePopupMenu->addAction(action_DeleteTableInstance);
    m_relockTableInstancePopupMenu->addMenu(m_tableInstancePopupMenu);

    // the locked func popup menus
    m_unlockFunctionPopupMenu->addAction(action_unlock);
    m_relockFunctionPopupMenu->addAction(action_relock);
    m_unlockFunctionPopupMenu->addMenu(m_functionPopupMenu);
    m_relockFunctionPopupMenu->addMenu(m_functionPopupMenu);

    // the locked proc popup menus
    m_unlockProcedurePopupMenu->addAction(action_unlock);
    m_unlockProcedurePopupMenu->addMenu(m_procedurePopupMenu);
    m_relockProcedurePopupMenu->addAction(action_relock);
    m_relockProcedurePopupMenu->addMenu(m_procedurePopupMenu);

    // the locked view popup menus
    m_unlockViewPopupMenu->addAction(action_unlock);
    m_relockViewPopupMenu->addAction(action_relock);

    // the locked dt popup menus
    m_unlockDTPopupMenu->addAction(action_unlock);
    m_relockDTPopupMenu->addAction(action_relock);

    // the locked diagr popup menus
    m_unlockDiagramPopupMenu->addAction(action_unlock);
    m_unlockDiagramPopupMenu->addMenu(m_diagramPopupMenu);
    m_relockDiagramPopupMenu->addAction(action_relock);
    m_relockDiagramPopupMenu->addMenu(m_diagramPopupMenu);

    m_resumePatchMenu->addAction(action_resumePatch);

    m_undeletePopupMenu->addAction(action_undelete);
}

QMenu* ContextMenuCollection::getUnLockMenuForClassUid(const QString& uid)
{
    QMap<QString, QMenu*> mapping;
    mapping.insert(uidTable.toUpper(), m_unlockTablePopupMenu);
    mapping.insert(uidTableInstance.toUpper(), m_unlockTableInstancePopupMenu);
    mapping.insert(uidDiagram.toUpper(), m_unlockDiagramPopupMenu);
    mapping.insert(uidProcedure.toUpper(), m_unlockProcedurePopupMenu);
    mapping.insert(uidFunction.toUpper(), m_unlockFunctionPopupMenu);
    mapping.insert(uidTrigger.toUpper(), m_unlockTriggerPopupMenu);
    mapping.insert(uidView.toUpper(), m_unlockViewPopupMenu);

    mapping.insert(uidStringDT.toUpper(), m_unlockDTPopupMenu);
    mapping.insert(uidNumericDT.toUpper(), m_unlockDTPopupMenu);
    mapping.insert(uidDateTimeDT.toUpper(), m_unlockDTPopupMenu);
    mapping.insert(uidBooleanDT.toUpper(), m_unlockDTPopupMenu);
    mapping.insert(uidMiscDT.toUpper(), m_unlockDTPopupMenu);
    mapping.insert(uidSpatialDT.toUpper(), m_unlockDTPopupMenu);
    mapping.insert(uidBlobDT.toUpper(), m_unlockDTPopupMenu);

    if(mapping.contains(uid.toUpper()))
    {
        return mapping[uid.toUpper()];
    }

    return 0;
}


QMenu* ContextMenuCollection::getReLockMenuForClassUid(const QString& uid)
{
    QMap<QString, QMenu*> mapping;
    mapping.insert(uidTable.toUpper(), m_relockTablePopupMenu);
    mapping.insert(uidTableInstance.toUpper(), m_relockTableInstancePopupMenu);
    mapping.insert(uidDiagram.toUpper(), m_relockDiagramPopupMenu);
    mapping.insert(uidProcedure.toUpper(), m_relockProcedurePopupMenu);
    mapping.insert(uidFunction.toUpper(), m_relockFunctionPopupMenu);
    mapping.insert(uidTrigger.toUpper(), m_relockTriggerPopupMenu);
    mapping.insert(uidView.toUpper(), m_relockViewPopupMenu);

    mapping.insert(uidStringDT.toUpper(), m_relockDTPopupMenu);
    mapping.insert(uidNumericDT.toUpper(), m_relockDTPopupMenu);
    mapping.insert(uidDateTimeDT.toUpper(), m_relockDTPopupMenu);
    mapping.insert(uidBooleanDT.toUpper(), m_relockDTPopupMenu);
    mapping.insert(uidMiscDT.toUpper(), m_relockDTPopupMenu);
    mapping.insert(uidSpatialDT.toUpper(), m_relockDTPopupMenu);
    mapping.insert(uidBlobDT.toUpper(), m_relockDTPopupMenu);

    if(mapping.contains(uid.toUpper()))
    {
        return mapping[uid.toUpper()];
    }

    return 0;
}

QMenu* ContextMenuCollection::getMenuForClassUid(const QString& uid)
{
    QMap<QString, QMenu*> mapping;
    mapping.insert(uidTable.toUpper(), m_tablePopupMenu);
    mapping.insert(uidTableInstance.toUpper(), m_tableInstancePopupMenu);
    mapping.insert(uidDiagram.toUpper(), m_diagramPopupMenu);
    mapping.insert(uidProcedure.toUpper(), m_procedurePopupMenu);
    mapping.insert(uidFunction.toUpper(), m_functionPopupMenu);
    mapping.insert(uidTrigger.toUpper(), m_triggerPopupMenu);
    mapping.insert(uidView.toUpper(), m_viewPopupMenu);

    mapping.insert(uidStringDT.toUpper(), m_datatypePopupMenu);
    mapping.insert(uidNumericDT.toUpper(), m_datatypePopupMenu);
    mapping.insert(uidDateTimeDT.toUpper(), m_datatypePopupMenu);
    mapping.insert(uidBooleanDT.toUpper(), m_datatypePopupMenu);
    mapping.insert(uidMiscDT.toUpper(), m_datatypePopupMenu);
    mapping.insert(uidSpatialDT.toUpper(), m_datatypePopupMenu);
    mapping.insert(uidBlobDT.toUpper(), m_datatypePopupMenu);

    if(mapping.contains(uid.toUpper()))
    {
        return mapping[uid.toUpper()];
    }

    return 0;
}
