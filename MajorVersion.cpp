#include <QtGui>

#include "MajorVersion.h"
#include "UserDataType.h"
#include "Table.h"
#include "Diagram.h"
#include "IconFactory.h"
#include "TableInstance.h"
#include "Project.h"
#include "ForeignKey.h"
#include "NameGenerator.h"

MajorVersion::MajorVersion(QTreeWidget* tree, QTreeWidget* dttree, ContextMenuEnabledTreeWidgetItem* projectItem, int ver, Project* p) :
        tablesItem(0), tableInstancesItem(0),
        dtsItem(0), versionItem(0), diagramsItem(0), finalSqlItem(0), version(""), m_dataTypes(), m_tables(), m_diagrams(), m_tableInstances(),
        m_tree(tree), m_dtTree(dttree), m_projectItem(projectItem), m_tablePopupMenu(0),
        action_RemoveTable(0), action_DuplicateTable(0), action_SpecializeTable(0), action_InstantiateTable(0),
        action_TableAddColumn(0), m_project(p)
{
    // make the dts sub item coming from the project
    QString v = QString::number(ver);
    version = v + ".0";
    v = QString("Ver: ") + v;
    v = v + ".0";

    createTreeItems();
}

MajorVersion::MajorVersion(QString verAsString, Project* p):version(verAsString), m_tree(0), m_projectItem(0), m_project(p)
{
}

void MajorVersion::populateTreeItems()
{
    // insert the user data types in the tree
    for(int i=0; i<m_dataTypes.size(); i++)
    {
        QStringList a(m_dataTypes[i]->getName());
        a << m_dataTypes[i]->sqlAsString();
        ContextMenuEnabledTreeWidgetItem* newDTItem = new ContextMenuEnabledTreeWidgetItem(getDtsItem(), a) ;
        QVariant var;
        var.setValue(*m_dataTypes[i]);
        newDTItem->setData(0, Qt::UserRole, var);
        newDTItem->setPopupMenu(getDatatypePopupMenu());
        // set the icon, add to the tree
        newDTItem->setIcon(0, m_dataTypes[i]->getIcon());
        m_dtTree->insertTopLevelItem(0, newDTItem);
        m_dataTypes[i]->setLocation(newDTItem);
    }
    m_dtTree->expandAll();
    m_dtTree->resizeColumnToContents(0);
    m_dtTree->resizeColumnToContents(1);

    // insert the tables
    for(int i=0; i<m_tables.size(); i++)
    {
        Table* tbl = m_tables[i];
        ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getTablesItem(), QStringList(tbl->getName())) ;

        QVariant var(tbl->getName());
        newTblsItem->setData(0, Qt::UserRole, var);
        newTblsItem->setPopupMenu(getTablePopupMenu());
        // set the icon, add to the tree
        newTblsItem->setIcon(0, IconFactory::getTablesIcon());
        m_tree->insertTopLevelItem(0, newTblsItem);

        // set the link to the tree
        tbl->setLocation(newTblsItem);
    }

    // now update the parents
    for(int i=0; i<m_tables.size(); i++)
    {
        if(m_tables[i]->getParent() != 0)
        {
            QTreeWidgetItem* p = m_tables[i]->getLocation();
            p->parent()->removeChild(p);
            m_tables[i]->getParent()->getLocation()->addChild(p);
        }
    }

    // insert the diagrams
    for(int i=0; i<m_diagrams.size(); i++)
    {
        Diagram* dI = m_diagrams[i];
        ContextMenuEnabledTreeWidgetItem* newDgramItem = new ContextMenuEnabledTreeWidgetItem(getDiagramsItem(), QStringList(dI->getName())) ;

        QVariant var(dI->getName());
        newDgramItem->setData(0, Qt::UserRole, var);
        // set the icon, add to the tree
        newDgramItem->setIcon(0, IconFactory::getDiagramIcon());
        newDgramItem->setPopupMenu(getDiagramPopupMenu());
        m_tree->insertTopLevelItem(0, newDgramItem);

        // set the link to the tree
        dI->setLocation(newDgramItem);
    }

    // insert the table instances (but only if this is an OOP project...)
    for(int i=0; i<m_tableInstances.size(); i++)
    {
        TableInstance* tI = m_tableInstances[i];
        ContextMenuEnabledTreeWidgetItem* newTabInstItem = new ContextMenuEnabledTreeWidgetItem(getTableInstancesItem(), QStringList(tI->getName())) ;

        QVariant var(tI->getName());
        newTabInstItem->setData(0, Qt::UserRole, var);
        newTabInstItem->setPopupMenu(getTableInstancePopupMenu());

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
    if(oop())
    {   // insert each table instance entry, since this is an OOP project
        for(int i=0; i<m_tableInstances.size(); i++)
        {
            TableInstance* tI = m_tableInstances[i];
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
        for(int i=0; i<m_tables.size(); i++)
        {
            Table* tbl = m_tables[i];
            ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(tbl->getName()+".sql")) ;

            QVariant var(tbl->getName());
            newTblsItem->setData(0, Qt::UserRole, var);
            newTblsItem->setPopupMenu(getTablePopupMenu());
            // set the icon, add to the tree
            newTblsItem->setIcon(0, IconFactory::getTablesIcon());
            m_tree->insertTopLevelItem(0, newTblsItem);
        }
    }
}

void MajorVersion::createTreeItems(QTreeWidget* tree, QTreeWidget* dtTree, ContextMenuEnabledTreeWidgetItem* projectIem)
{
    if(tree)
    {
        m_tree = tree;
    }
    if(dtTree)
    {
        m_dtTree = dtTree;
    }
    if(projectIem)
    {
        m_projectItem = projectIem;
    }

    versionItem = new ContextMenuEnabledTreeWidgetItem(m_projectItem, QStringList(QString("Ver: ") + version)) ;
    versionItem->setIcon(0, IconFactory::getVersionIcon());
    m_tree->addTopLevelItem(versionItem);

    // make the dts sub item coming from the project
    dtsItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, QStringList(QObject::tr("Data Types"))) ;
    dtsItem->setIcon(0, IconFactory::getDataTypesIcon());
    m_dtTree->addTopLevelItem(dtsItem);

    // make the tables sub item coming from the project
    tablesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Table templates"))) ;
    tablesItem->setIcon(0, IconFactory::getTablesIcon());
    m_tree->addTopLevelItem(tablesItem);

    // make the views sub item coming from the project
    tableInstancesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Tables"))) ;
    tableInstancesItem->setIcon(0, IconFactory::getTabinstIcon());
    m_tree->addTopLevelItem(tableInstancesItem);

    diagramsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Diagrams"))) ;
    diagramsItem->setIcon(0, IconFactory::getDiagramIcon());
    m_tree->addTopLevelItem(diagramsItem);

    ContextMenuEnabledTreeWidgetItem* codeItem= new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Code"))) ;
    codeItem->setIcon(0, IconFactory::getCodeIcon());
    m_tree->addTopLevelItem(codeItem);

    finalSqlItem= new ContextMenuEnabledTreeWidgetItem(codeItem, QStringList(QObject::tr("SQL"))) ;
    finalSqlItem->setIcon(0, IconFactory::getSqlIcon());
    m_tree->addTopLevelItem(finalSqlItem);

    // popup menus
    m_tablePopupMenu = new QMenu();
    m_tableInstancePopupMenu = new QMenu();
    m_datatypePopupMenu = new QMenu();
    m_diagramPopupMenu = new QMenu();

    // actions
    action_RemoveTable = new QAction(QObject::tr("Delete table"), 0);
    action_RemoveTable->setIcon(IconFactory::getRemoveIcon());
    action_DuplicateTable = new QAction(QObject::tr("Duplicate table"), 0);
    action_TableAddColumn = new QAction(QObject::tr("Add column"), 0);
    action_SpecializeTable = new QAction(QObject::tr("Specialize table"), 0);
    action_InstantiateTable = new QAction(QObject::tr("Instantiate table"), 0);
    action_DeleteTableInstance = new QAction(QObject::tr("Delete instance"), 0);
    action_DeleteTableInstance->setIcon(IconFactory::getRemoveIcon());
    action_RenameTableInstance = new QAction(QObject::tr("Rename instance"), 0);
    action_DeleteDataType = new QAction(QObject::tr("Delete datatype"), 0);
    action_DeleteDataType->setIcon(IconFactory::getRemoveIcon());
    action_DuplicateDataType = new QAction(QObject::tr("Duplicate datatype"), 0);
    action_DeleteDiagram = new QAction(QObject::tr("Delete diagram"), 0);
    action_DeleteDiagram->setIcon(IconFactory::getRemoveIcon());
    action_RenameDiagram = new QAction(QObject::tr("Rename diagram"), 0);

    // populate the table popup menu
    m_tablePopupMenu->addAction(action_TableAddColumn);
    m_tablePopupMenu->addSeparator();
    m_tablePopupMenu->addAction(action_RemoveTable);
    m_tablePopupMenu->addAction(action_DuplicateTable);
    m_tablePopupMenu->addAction(action_SpecializeTable);
    m_tablePopupMenu->addSeparator();
    m_tablePopupMenu->addAction(action_InstantiateTable);

    // populate the table instances popup menu
    m_tableInstancePopupMenu->addAction(action_DeleteTableInstance);
    m_tableInstancePopupMenu->addAction(action_RenameTableInstance);

    // datatypes popup menu
    m_datatypePopupMenu->addAction(action_DeleteDataType);
    m_datatypePopupMenu->addAction(action_DuplicateDataType);

    // diagrams popup menu
    m_diagramPopupMenu->addAction(action_DeleteDiagram);
    m_diagramPopupMenu->addAction(action_RenameDiagram);
}

void MajorVersion::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement majorVersionElement = doc.createElement("MajorVersion");      // will hold the data in this element

    QDomElement versionElement = doc.createElement("Version");        // the version node
    QDomText versionNode = doc.createTextNode(version);
    versionElement.appendChild(versionNode);
    majorVersionElement.appendChild(versionElement);

    // the data types
    {
    QDomElement dataTypesElement = doc.createElement("DataTypes");
    for(int i=0; i< m_dataTypes.size(); i++)
    {
        m_dataTypes[i]->serialize(doc, dataTypesElement);
    }
    majorVersionElement.appendChild(dataTypesElement);
    }

    // the tables
    {
    QDomElement tablesElement = doc.createElement("Tables");
    for(int i=0; i< m_tables.size(); i++)
    {
        m_tables[i]->serialize(doc, tablesElement);
    }
    majorVersionElement.appendChild(tablesElement);
    }

    // the diagrams
    {
    QDomElement diagramsElement = doc.createElement("Diagrams");
    for(int i=0; i< m_diagrams.size(); i++)
    {
        m_diagrams[i]->serialize(doc, diagramsElement);
    }
    majorVersionElement.appendChild(diagramsElement);
    }

    // the table instances
    {
    QDomElement tableInstancesElement = doc.createElement("TableInstances");
    for(int i=0; i< m_tableInstances.size(); i++)
    {
        m_tableInstances[i]->serialize(doc, tableInstancesElement);
    }
    majorVersionElement.appendChild(tableInstancesElement);
    }


    parent.appendChild(majorVersionElement);

}

inline void MajorVersion::addNewDataType(UserDataType* dt)
{
    m_dataTypes.append(dt);
}

bool MajorVersion::hasDataType(const QString& name) const
{
    for(int i=0; i < m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return true;
        }
    }
    return false;
}

UserDataType* MajorVersion::getDataType(const QString& name)
{
    for(int i=0; i< m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return const_cast<UserDataType*>(m_dataTypes.at(i));  // yeah, this sucks
        }
    }
    return 0;
}

int MajorVersion::getDataTypeIndex(const QString& name)
{
    for(int i=0; i < m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return i;
        }
    }
    return -1;
}

inline void MajorVersion::addTable(Table *t)
{
    m_tables.append(t);
}

inline void MajorVersion::addDiagram(Diagram* d)
{
    m_diagrams.append(d);
}

inline bool MajorVersion::hasTable(Table *t)
{
    return m_tables.indexOf(t) >= 0;
}

inline bool MajorVersion::hasTable(const QString& tb)
{
    for(int i=0; i<m_tables.size(); i++)
    {
        if(QString::compare(m_tables[i]->getName(), tb, Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }
    return false;
}


// TODO: 1. Make the Table class to come from the named item
//       2. the two methods below could be very easily templated after this

Table* MajorVersion::getTable(const QString &name)
{
    for(int i=0; i< m_tables.size(); i++)
    {
        if(m_tables[i]->getName() == name)
        {
            return m_tables[i];
        }
    }
    return 0;
}

Diagram* MajorVersion::getDiagram(const QString& name)
{
    for(int i=0; i<m_diagrams.size(); i++)
    {
        if(m_diagrams[i]->getName() == name)
        {
            return m_diagrams[i];
        }
    }

    return 0;
}

inline const QVector<UserDataType*>& MajorVersion::getDataTypes() const
{
    return m_dataTypes;
}

inline const QVector<Table*>& MajorVersion::getTables() const
{
    return m_tables;
}

void MajorVersion::purgeSentencedTableInstances()
{
    int i=0;
    while(i<m_tableInstances.size())
    {
        if(m_tableInstances.at(i)->sentenced())
        {
            TableInstance* tinst = m_tableInstances.at(i);
            m_tableInstances.remove(i);
            delete tinst;
        }
        else
        {
            i++;
        }
    }
}

void MajorVersion::deleteTableInstance(TableInstance *tinst)
{
    tinst->sentence();
    // now find which table instances were created because of this and remove them
    QVector<TableInstance*>& insted = tinst->getInstantiatedTableInstances();
    for(int i=0; i<insted.size(); i++)
    {
        // except if there are other table instances that have it as instantiated
        bool someoneElseAlsoInstantiatedThis = false;
        for(int j=0; j<m_tableInstances.size(); j++)
        {
            if(tinst != m_tableInstances.at(j) )
            {
                for(int k=0; k<m_tableInstances.at(j)->getInstantiatedTableInstances().size(); k++)
                {
                    if(m_tableInstances.at(j)->getInstantiatedTableInstances().at(k) == insted.at(i))
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
                deleteTableInstance(insted.at(i));
            }
        }
    }
}

void MajorVersion::deleteTable(Table *tab)
{
    int tabIndex = -1;
    QString incomingForeignKeys = "";
    for(int i=0; i<m_tables.size(); i++)
    {
        if(m_tables[i]->getForeignKeyToTable(tab->getName()) != 0)
        {
            incomingForeignKeys+= "\n - " + m_tables[i]->getName();
        }

        if(m_tables[i]->getName() == tab->getName())
        {
            tabIndex = i;
        }

    }
    if(incomingForeignKeys.length() > 0)
    {
        QMessageBox::warning(0, QObject::tr("Foreign keys found"),
                             QObject::tr("Cannot delete this table since the following tables are referencing it through a foreign key: ") +
                             incomingForeignKeys +
                             QObject::tr("\nFirstly remove the foreign keys, then delete the table."), QMessageBox::Ok);
        return;
    }

    for(int i=0; i<m_diagrams.size(); i++)
    {
        m_diagrams[i]->removeTable(tab->getName());
    }

    delete m_tables[tabIndex]->getLocation();
    m_tables.remove(tabIndex);
}



void MajorVersion::removeForeignKeyFromDiagrams(ForeignKey* fkToRemove)
{
    for(int i=0; i<m_diagrams.size(); i++)
    {
        m_diagrams[i]->forcefullyRemoveForeignKey(fkToRemove);
    }
}

Table* MajorVersion::duplicateTable(Table *src)
{
    Table* dup = new Table(*src);
    dup->setName(NameGenerator::getNextTableNameFromVersion(this, src->getName()+"_copy"));
    addTable(dup);
    return dup;
}

void MajorVersion::setupTableParentChildRelationships()
{
    for(int i=0; i<m_tables.size(); i++)
    {
        if(m_tables[i]->getTempTabName().length() > 0)
        {
            m_tables[i]->setParent(getTable(m_tables[i]->getTempTabName()));
        }
    }
}

TableInstance* MajorVersion::instantiateTable(Table* tab, bool reason)
{
    TableInstance* tabInst = new TableInstance(tab, reason);
    TableInstance* other = getTableInstance(tabInst->getName());
    if(other && other != tabInst)
    {
        tabInst->setName(NameGenerator::generateUniqueTableInstanceName(this, tabInst->getName()));
    }
    m_tableInstances.append(tabInst);
    return tabInst;
}

DatabaseEngine* MajorVersion::getDatabaseEngine()
{
    return m_project->getEngine();
}

bool MajorVersion::oop()
{
    return m_project->oopProject();
}

TableInstance* MajorVersion::getTableInstance(const QString& name)
{
    for(int i=0; i<m_tableInstances.size(); i++)
    {
        if(m_tableInstances.at(i)->getName() == name)
        {
            return m_tableInstances.at(i);
        }
    }
    return 0;
}

void MajorVersion::deleteDataType(const QString& dtName)
{
    for(int i=0; i<m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == dtName)
        {
            m_dataTypes.remove(i);
            return;
        }
    }
}

UserDataType* MajorVersion::duplicateDataType(const QString& name)
{
    UserDataType* src = 0;
    for(int i=0; i<m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            src = m_dataTypes.at(i);
            break;
        }
    }
    if(!src)
    {
        return 0;
    }
    UserDataType* dt = new UserDataType(*src);
    dt->setName(NameGenerator::generateUniqueDatatypeName(this, dt->getName()));
    addNewDataType(dt);
    return dt;
}

void MajorVersion::deleteDiagram(const QString& name)
{
    Diagram* dgr = getDiagram(name);
    m_diagrams.remove(m_diagrams.indexOf(dgr));
    delete dgr->getLocation();
    delete dgr;
}
