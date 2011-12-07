#include "ContextMenuCollection.h"
#include "IconFactory.h"
#include "core_ConnectionManager.h"

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
    action_connectionConnect = new QAction(QObject::tr("Connect"), 0);
    action_connectionDelete = new QAction(QObject::tr("Delete"), 0);
    action_connectionEdit = new QAction(QObject::tr("Edit"), 0);
    action_connectionBrowse = new QAction(QObject::tr("Browse"), 0);
    action_browsedTableInjectIntoSolution = new QAction(QObject::tr("Add to current solution"), 0);
    action_browsedTableBrowse = new QAction(QObject::tr("Browse table data"), 0);
    action_browsedTableView = new QAction(QObject::tr("View table details"), 0);

    // populate the table popup menu
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
    m_tableInstancePopupMenu->addAction(action_DeleteTableInstance);
    m_tableInstancePopupMenu->addAction(action_RenameTableInstance);

    // datatypes popup menu
    m_datatypePopupMenu->addAction(action_DeleteDataType);
    m_datatypePopupMenu->addAction(action_DuplicateDataType);

    // diagrams popup menu
    m_diagramPopupMenu->addAction(action_DeleteDiagram);
    m_diagramPopupMenu->addAction(action_RenameDiagram);

    //tables popup menu
    m_tablesPopupMenu->addAction(action_AddTable);
    m_tablesPopupMenu->addAction(action_PasteTable);

    // table instances
    m_tableInstancesPopupMenu->addAction(action_AddTableInstance);

    // diagrams
    m_diagramsPopupMenu->addAction(action_AddDiagram);

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
    m_connectionPopupMenu->addAction(action_connectionConnect);
    m_connectionPopupMenu->addAction(action_connectionDelete);
    m_connectionPopupMenu->addAction(action_connectionEdit);
    m_connectionPopupMenu->addAction(action_connectionBrowse);

    // browsed table
    m_browsedTablePopupMenu->addAction(action_browsedTableInjectIntoSolution);
    m_browsedTablePopupMenu->addAction(action_browsedTableBrowse);
    m_browsedTablePopupMenu->addAction(action_browsedTableView);

    // popup menu for the table instances
    m_createTableInstancesPopup->clear();
}
