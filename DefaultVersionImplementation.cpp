#include "DefaultVersionImplementation.h"

#include "ContextMenuCollection.h"
#include "UserDataType.h"
#include "Table.h"
#include "Diagram.h"
#include "IconFactory.h"
#include "TableInstance.h"
#include "Project.h"
#include "ForeignKey.h"
#include "NameGenerator.h"
#include "VersionGuiElements.h"

#include <QtGui>

DefaultVersionImplementation::DefaultVersionImplementation(QTreeWidget* tree, QTreeWidget* dttree, ContextMenuEnabledTreeWidgetItem* projectItem, Project* p)
    : version(""), m_dataTypes(), m_tables(), m_diagrams(), m_tableInstances(), m_tree(tree), m_dtTree(dttree), m_projectItem(projectItem), m_project(p), m_guiElements(0)
{
}

void DefaultVersionImplementation::createTreeItems(QTreeWidget* tree, QTreeWidget* dtTree, ContextMenuEnabledTreeWidgetItem* projectIem)
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

    m_guiElements = new VersionGuiElements(m_tree, m_dtTree, this);
    m_guiElements->createGuiElements(m_projectItem);
}

inline void DefaultVersionImplementation::addNewDataType(UserDataType* dt)
{
    m_dataTypes.append(dt);
}

bool DefaultVersionImplementation::hasDataType(const QString& name) const
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

UserDataType* DefaultVersionImplementation::getDataType(const QString& name)
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

int DefaultVersionImplementation::getDataTypeIndex(const QString& name)
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

inline void DefaultVersionImplementation::addTable(Table *t)
{
    m_tables.append(t);
}

inline void DefaultVersionImplementation::addDiagram(Diagram* d)
{
    m_diagrams.append(d);
}

inline bool DefaultVersionImplementation::hasTable(Table *t)
{
    return m_tables.indexOf(t) >= 0;
}

inline bool DefaultVersionImplementation::hasTable(const QString& tb)
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

Table* DefaultVersionImplementation::getTable(const QString &name)
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

Diagram* DefaultVersionImplementation::getDiagram(const QString& name)
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

inline const QVector<UserDataType*>& DefaultVersionImplementation::getDataTypes() const
{
    return m_dataTypes;
}

inline const QVector<Table*>& DefaultVersionImplementation::getTables() const
{
    return m_tables;
}

void DefaultVersionImplementation::purgeSentencedTableInstances()
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

void DefaultVersionImplementation::deleteTableInstance(TableInstance *tinst)
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

void DefaultVersionImplementation::deleteTable(Table *tab)
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
    if(tabIndex == -1) return;
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

    // and now we should delete the table instances that were created absed on this table, and the SQLs too...
    int i=0;
    while(i<m_tableInstances.size())
    {
        if(m_tableInstances.at(i)->table()->getName() == tab->getName())
        {
            delete m_tableInstances.at(i)->getLocation();
            m_tableInstances.at(i)->onDelete();
            m_tableInstances.remove(i);
        }
        else
        {
            i++;
        }
    }


    delete m_tables[tabIndex]->getLocation();
    m_tables.remove(tabIndex);
}



void DefaultVersionImplementation::removeForeignKeyFromDiagrams(ForeignKey* fkToRemove)
{
    for(int i=0; i<m_diagrams.size(); i++)
    {
        m_diagrams[i]->forcefullyRemoveForeignKey(fkToRemove);
    }
}

Table* DefaultVersionImplementation::duplicateTable(Table *src)
{
    Table* dup = new Table(*src);
    dup->setName(NameGenerator::getNextTableNameFromVersion(this, src->getName()+"_copy"));
    addTable(dup);
    return dup;
}

void DefaultVersionImplementation::setupTableParentChildRelationships()
{
    for(int i=0; i<m_tables.size(); i++)
    {
        if(m_tables[i]->getTempTabName().length() > 0)
        {
            m_tables[i]->setParent(getTable(m_tables[i]->getTempTabName()));
        }
    }
}

TableInstance* DefaultVersionImplementation::instantiateTable(Table* tab, bool reason)
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

TableInstance* DefaultVersionImplementation::getTableInstance(const QString& name)
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

void DefaultVersionImplementation::deleteDataType(const QString& dtName)
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

UserDataType* DefaultVersionImplementation::duplicateDataType(const QString& name)
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

void DefaultVersionImplementation::deleteDiagram(const QString& name)
{
    Diagram* dgr = getDiagram(name);
    m_diagrams.remove(m_diagrams.indexOf(dgr));
    delete dgr->getLocation();
    delete dgr;
}
