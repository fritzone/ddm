#include "VersionGuiElements.h"
#include "ContextMenuEnabledTreeWidget.h"
#include "IconFactory.h"
#include "Version.h"
#include "UserDataType.h"
#include "ContextMenuCollection.h"
#include "Table.h"
#include "Diagram.h"
#include "TableInstance.h"
#include "Workspace.h"

#include <QVector>

VersionGuiElements::VersionGuiElements(QTreeWidget* projTree, QTreeWidget* dtTree, Version* v) : tablesItem(0), tableInstancesItem(0), versionItem(0), diagramsItem(0), finalSqlItem(0), dtsItem(0),
    m_tree(projTree), m_dtTree(dtTree), m_version(v)
{
}

void VersionGuiElements::createGuiElements(ContextMenuEnabledTreeWidgetItem* projectItem)
{

    versionItem = new ContextMenuEnabledTreeWidgetItem(projectItem, QStringList(QString("Ver: ") + m_version->getVersionText())) ;
    versionItem->setIcon(0, IconFactory::getVersionIcon());
    m_tree->addTopLevelItem(versionItem);

    // make the tables sub item coming from the version
    tablesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Table templates"))) ;
    tablesItem->setIcon(0, IconFactory::getTablesIcon());
    m_tree->addTopLevelItem(tablesItem);

    // make the views sub item coming from the version
    tableInstancesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Tables"))) ;
    tableInstancesItem->setIcon(0, IconFactory::getTabinstIcon());
    m_tree->addTopLevelItem(tableInstancesItem);

    diagramsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Diagrams"))) ;
    diagramsItem->setIcon(0, IconFactory::getDiagramIcon());
    m_tree->addTopLevelItem(diagramsItem);

    // last one: SQLs
    ContextMenuEnabledTreeWidgetItem* codeItem= new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Code"))) ;
    codeItem->setIcon(0, IconFactory::getCodeIcon());
    m_tree->addTopLevelItem(codeItem);

    finalSqlItem= new ContextMenuEnabledTreeWidgetItem(codeItem, QStringList(QObject::tr("SQL"))) ;
    finalSqlItem->setIcon(0, IconFactory::getSqlIcon());
    m_tree->addTopLevelItem(finalSqlItem);

    // make the dts sub item coming from the project
    dtsItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, QStringList(QObject::tr("Data Types"))) ;
    dtsItem->setIcon(0, IconFactory::getDataTypesIcon());
    m_dtTree->addTopLevelItem(dtsItem);
}

void VersionGuiElements::populateTreeItems()
{
    // insert the user data types in the tree
    const QVector<UserDataType*>& dataTypes = m_version->getDataTypes();
    for(int i=0; i<dataTypes.size(); i++)
    {
        QStringList a(dataTypes[i]->getName());
        a << dataTypes[i]->sqlAsString();
        ContextMenuEnabledTreeWidgetItem* newDTItem = new ContextMenuEnabledTreeWidgetItem(getDtsItem(), a) ;
        QVariant var;
        var.setValue(*dataTypes[i]);
        newDTItem->setData(0, Qt::UserRole, var);
        newDTItem->setPopupMenu(ContextMenuCollection::getInstance()->getDatatypePopupMenu());
        // set the icon, add to the tree
        newDTItem->setIcon(0, dataTypes[i]->getIcon());
        m_dtTree->insertTopLevelItem(0, newDTItem);
        dataTypes[i]->setLocation(newDTItem);
    }
    m_dtTree->expandAll();
    m_dtTree->resizeColumnToContents(0);
    m_dtTree->resizeColumnToContents(1);

    // insert the tables
    const QVector<Table*>& tables = m_version->getTables();
    for(int i=0; i<tables.size(); i++)
    {
        Table* tbl = tables.at(i);
        ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getTablesItem(), QStringList(tbl->getName())) ;

        QVariant var(tbl->getName());
        newTblsItem->setData(0, Qt::UserRole, var);
        newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
        // set the icon, add to the tree
        newTblsItem->setIcon(0, IconFactory::getTablesIcon());
        m_tree->insertTopLevelItem(0, newTblsItem);

        // set the link to the tree
        tbl->setLocation(newTblsItem);
    }

    // now update the parents
    for(int i=0; i<tables.size(); i++)
    {
        if(tables[i]->getParent() != 0)
        {
            QTreeWidgetItem* p = tables[i]->getLocation();
            p->parent()->removeChild(p);
            tables[i]->getParent()->getLocation()->addChild(p);
        }
    }

    // insert the diagrams
    const QVector<Diagram*>& diagrams = m_version->getDiagrams();
    for(int i=0; i<diagrams.size(); i++)
    {
        Diagram* dI = diagrams.at(i);
        ContextMenuEnabledTreeWidgetItem* newDgramItem = new ContextMenuEnabledTreeWidgetItem(getDiagramsItem(), QStringList(dI->getName())) ;

        QVariant var(dI->getName());
        newDgramItem->setData(0, Qt::UserRole, var);
        // set the icon, add to the tree
        newDgramItem->setIcon(0, IconFactory::getDiagramIcon());
        newDgramItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramPopupMenu());
        m_tree->insertTopLevelItem(0, newDgramItem);

        // set the link to the tree
        dI->setLocation(newDgramItem);
    }

    // insert the table instances (but only if this is an OOP project...)
    const QVector<TableInstance*> &  tableInstances = m_version->getTableInstances();
    for(int i=0; i<tableInstances.size(); i++)
    {
        TableInstance* tI = tableInstances.at(i);
        ContextMenuEnabledTreeWidgetItem* newTabInstItem = new ContextMenuEnabledTreeWidgetItem(getTableInstancesItem(), QStringList(tI->getName())) ;

        QVariant var(tI->getName());
        newTabInstItem->setData(0, Qt::UserRole, var);
        newTabInstItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableInstancePopupMenu());

        // set the icon, add to the tree
        if(tI->instantiatedBecuaseOfRkReference())
        {
            newTabInstItem->setIcon(0, IconFactory::getTabinstLockIcon());
        }
        else
        {
            newTabInstItem->setIcon(0, IconFactory::getTabinstIcon());
        }

        m_tree->insertTopLevelItem(0, newTabInstItem);

        // set the link to the tree
        tI->setLocation(newTabInstItem);
    }

    // now populate the "Code" tree items by adding the SQLs of the tables or table instances
    if(Workspace::getInstance()->currentProjectIsOop())
    {   // insert each table instance entry, since this is an OOP project
        for(int i=0; i<tableInstances.size(); i++)
        {
            TableInstance* tI = tableInstances[i];
            ContextMenuEnabledTreeWidgetItem* tabInstSqlItem = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(tI->getName()+".sql")) ;

            QVariant var(tI->getName());
            tabInstSqlItem->setData(0, Qt::UserRole, var);

            // set the icon, add to the tree
            if(tI->instantiatedBecuaseOfRkReference())
            {
                tabInstSqlItem->setIcon(0, IconFactory::getTabinstLockIcon());
            }
            else
            {
                tabInstSqlItem->setIcon(0, IconFactory::getTabinstIcon());
            }
            m_tree->insertTopLevelItem(0, tabInstSqlItem);
        }
    }
    else
    {
        // insert the tables
        for(int i=0; i<tables.size(); i++)
        {
            Table* tbl = tables.at(i);
            ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(tbl->getName()+".sql")) ;

            QVariant var(tbl->getName());
            newTblsItem->setData(0, Qt::UserRole, var);
            newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
            // set the icon, add to the tree
            newTblsItem->setIcon(0, IconFactory::getTablesIcon());
            m_tree->insertTopLevelItem(0, newTblsItem);
        }
    }
}

