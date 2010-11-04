#include "ContextMenuCollection.h"
#include "IconFactory.h"

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
}

