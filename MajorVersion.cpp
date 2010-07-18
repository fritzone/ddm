#include <QtGui>

#include "MajorVersion.h"
#include "UserDataType.h"
#include "Table.h"
#include "Diagram.h"
#include "IconFactory.h"

MajorVersion::MajorVersion(QTreeWidget* tree, QTreeWidget* dttree, ContextMenuEnabledTreeWidgetItem* projectItem, int ver) :
        m_tree(tree), m_dtTree(dttree), m_projectItem(projectItem), action_RemoveTable(0)
{
    // make the dts sub item coming from the project
    QString v = QString::number(ver);
    version = v + ".0";
    v = QString("Ver: ") + v;
    v = v + ".0";

    createTreeItems();
}

MajorVersion::MajorVersion(QString verAsString):version(verAsString), m_tree(0), m_projectItem(0)
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
        // set the icon, add to the tree
        newDTItem->setIcon(0, m_dataTypes[i]->getIcon());
        m_dtTree->insertTopLevelItem(0, newDTItem);
        m_dataTypes[i]->setLocation(newDTItem);
    }
    m_dtTree->expandAll();

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
        m_tree->insertTopLevelItem(0, newDgramItem);

        // set the link to the tree
        dI->setLocation(newDgramItem);
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

    QIcon tablesIcon(":/images/actions/images/small/table.png");
    QIcon tabInstanceIcon(":/images/actions/images/small/tabinst.png");
    QIcon dtsIcon(":/images/actions/images/small/datatypes.PNG");
    QIcon versionIcon(":/images/actions/images/small/version.PNG");

    versionItem = new ContextMenuEnabledTreeWidgetItem(m_projectItem, QStringList(QString("Ver: ") + version)) ;
    versionItem->setIcon(0, versionIcon);
    m_tree->addTopLevelItem(versionItem);

    // make the dts sub item coming from the project
    dtsItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, QStringList(QObject::tr("Data Types"))) ;
    dtsItem->setIcon(0, dtsIcon);
    m_dtTree->addTopLevelItem(dtsItem);

    // make the tables sub item coming from the project
    tablesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Table types"))) ;
    tablesItem->setIcon(0, tablesIcon);
    m_tree->addTopLevelItem(tablesItem);

    // make the views sub item coming from the project
    tableInstancesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Tables"))) ;
    tableInstancesItem->setIcon(0, tabInstanceIcon);
    m_tree->addTopLevelItem(tableInstancesItem);

    diagramsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Diagrams"))) ;
    diagramsItem->setIcon(0, IconFactory::getDiagramIcon());
    m_tree->addTopLevelItem(diagramsItem);

    m_tablePopupMenu = new QMenu();

    action_RemoveTable = new QAction("Delete table", 0);
    action_DuplicateTable = new QAction("Duplicate table", 0);
    action_TableAddColumn = new QAction("Add column", 0);
    action_SpecializeTable = new QAction("Specialize table", 0);

    QIcon remove(":/images/actions/images/small/remove.png");
    action_RemoveTable->setIcon(remove);

    m_tablePopupMenu->addAction(action_TableAddColumn);
    m_tablePopupMenu->addSeparator();
    m_tablePopupMenu->addAction(action_RemoveTable);
    m_tablePopupMenu->addAction(action_DuplicateTable);
    m_tablePopupMenu->addAction(action_SpecializeTable);
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
        QMessageBox::warning(0, "Foreign keys found",
                              "Cannot delete this table since the following tables are referencing it through a foreign key: " + incomingForeignKeys + "\nFirstly remove the foreign keys, then delete the table.", QMessageBox::Ok);
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
    dup->setName(src->getName()+"_copy");
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
