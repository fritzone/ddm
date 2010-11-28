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
#include "Workspace.h"
#include "Configuration.h"
#include "DatabaseEngine.h"

#include <QtGui>

DefaultVersionImplementation::DefaultVersionImplementation(QTreeWidget* tree, QTreeWidget* dttree, ContextMenuEnabledTreeWidgetItem* projectItem, Project* p)
    : version(""), m_data(), m_tree(tree), m_dtTree(dttree), m_projectItem(projectItem), m_project(p), m_guiElements(0)
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

void DefaultVersionImplementation::addNewDataType(UserDataType* dt)
{
    int i = 0;
    while(i < m_data.m_dataTypes.size())
    {
        if(m_data.m_dataTypes.at(i)->getType() != dt->getType()) i++;
        else break;
    }

    m_data.m_dataTypes.insert(i, dt);

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

inline void DefaultVersionImplementation::addTable(Table *t)
{
    m_data.m_tables.append(t);
}

inline void DefaultVersionImplementation::addDiagram(Diagram* d)
{
    m_data.m_diagrams.append(d);
}

inline bool DefaultVersionImplementation::hasTable(Table *t)
{
    return m_data.m_tables.indexOf(t) >= 0;
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


// TODO: 1. Make the Table class to come from the named item
//       2. the two methods below could be very easily templated after this

Table* DefaultVersionImplementation::getTable(const QString &name)
{
    for(int i=0; i< m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getName() == name)
        {
            return m_data.m_tables[i];
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
            TableInstance* tinst = m_data.m_tableInstances.at(i);
            m_data.m_tableInstances.remove(i);
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
                deleteTableInstance(insted.at(i));
            }
        }
    }
}

void DefaultVersionImplementation::deleteTable(Table *tab)
{
    int tabIndex = -1;
    QString incomingForeignKeys = "";
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        if(m_data.m_tables[i]->getForeignKeyToTable(tab->getName()) != 0)
        {
            incomingForeignKeys+= "\n - " + m_data.m_tables[i]->getName();
        }

        if(m_data.m_tables[i]->getName() == tab->getName())
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

    for(int i=0; i<m_data.m_diagrams.size(); i++)
    {
        m_data.m_diagrams[i]->removeTable(tab->getName());
    }

    // and now we should delete the table instances that were created absed on this table, and the SQLs too...
    int i=0;
    while(i<m_data.m_tableInstances.size())
    {
        if(m_data.m_tableInstances.at(i)->table()->getName() == tab->getName())
        {
            delete m_data.m_tableInstances.at(i)->getLocation();
            m_data.m_tableInstances.at(i)->onDelete();
            m_data.m_tableInstances.remove(i);
        }
        else
        {
            i++;
        }
    }


    delete m_data.m_tables[tabIndex]->getLocation();
    m_data.m_tables.remove(tabIndex);
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
    dup->setName(NameGenerator::getNextTableNameFromVersion(this, src->getName()+"_copy"));
    addTable(dup);
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
    TableInstance* tabInst = new TableInstance(tab, becauseOfReference);
    TableInstance* other = getTableInstance(tabInst->getName());        // just to see that we have another instance of this table
    if(other && other != tabInst)
    {   // and in this case generate a new name for the current one
        tabInst->setName(NameGenerator::generateUniqueTableInstanceName(this, tabInst->getName()));
    }
    m_data.m_tableInstances.append(tabInst);
    return tabInst;
}

TableInstance* DefaultVersionImplementation::getTableInstance(const QString& name)
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

void DefaultVersionImplementation::deleteDataType(const QString& dtName)
{
    for(int i=0; i<m_data.m_dataTypes.size(); i++)
    {
        if(m_data.m_dataTypes.at(i)->getName() == dtName)
        {
            m_data.m_dataTypes.remove(i);
            return;
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
    dt->setName(NameGenerator::generateUniqueDatatypeName(this, dt->getName()));
    addNewDataType(dt);
    return dt;
}

void DefaultVersionImplementation::deleteDiagram(const QString& name)
{
    Diagram* dgr = getDiagram(name);
    m_data.m_diagrams.remove(m_data.m_diagrams.indexOf(dgr));
    delete dgr->getLocation();
    delete dgr;
}

QVector<Table*> DefaultVersionImplementation::getTablesReferencingAColumnThroughForeignKeys(const Column* col)
{
    QVector<Table*> result;
    for(int i=0; i<m_data.m_tables.size(); i++)
    {
        for(int j=0; j<m_data.m_tables.at(i)->getFks().size(); j++)
        {
            if(m_data.m_tables.at(i)->getFks().at(j)->foreignColumns().contains(col))
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
    for(int j=0; j<tab->getFks().size(); j++)
    {
        ForeignKey* fkJ = tab->getFks()[j];
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
                if(getTables().at(l)->getName() == assK->getSLocalTable())
                {
                    assK->setLocalTable(getTables().at(l));
                }
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

QList<QString> DefaultVersionImplementation::getSqlScript(const QString& codepage)
{
    QList<QString> finalSql;
    QHash<QString, QString> opts = Configuration::instance().sqlGenerationOptions();
    bool upcase = opts.contains("Case") && opts["Case"] == "Upper";
    opts["FKSposition"] = "OnlyInternal";

    if(Workspace::getInstance()->currentProjectIsOop())   // list the table instances' SQL
    {
        for(int i=0; i<getTableInstances().size(); i++)
        {
            QStringList sql = getTableInstances().at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts, codepage);
            finalSql << sql;
        }

        for(int i=0; i<getTableInstances().size(); i++)
        {
            for(int j=0; j<getTableInstances().at(i)->table()->getForeignKeyCommands().size(); j++)
            {
                QString f = upcase?"ALTER TABLE ":"alter table ";
                f += getTableInstances().at(i)->getName();
                f += upcase?" ADD ":" add ";
                f += getTableInstances().at(i)->table()->getForeignKeyCommands().at(j);
                f += ";\n";

                finalSql << f;
            }
        }
    }
    else    // list table's SQL
    {
        for(int i=0; i<getTables().size(); i++)
        {
            QStringList sql = getTables().at(i)->generateSqlSource(m_project->getEngine()->getSqlGenerator(), opts, codepage);
            finalSql << sql;
        }

        for(int i=0; i<getTables().size(); i++)
        {
            for(int j=0; j<getTables().at(i)->getForeignKeyCommands().size(); j++)
            {
                QString f = upcase?"ALTER TABLE ":"alter table ";
                f += getTables().at(i)->getName();
                f += upcase?" ADD ":" add ";
                f += getTables().at(i)->getForeignKeyCommands().at(j);
                f += ";\n";

                finalSql << f;
            }
        }
    }
    return finalSql;
}

const QVector<Diagram*>& DefaultVersionImplementation::getDiagrams() const
{
    return m_data.m_diagrams;
}

void DefaultVersionImplementation::addTableInstance(TableInstance* inst)
{
    m_data.m_tableInstances.append(inst);
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
    UserDataType* newUdt = new UserDataType(//type + (size.length()>0?" ("+ size+")":"") + (defaultValue.length() > 0? ("=" + defaultValue) : "") + (nullable=="NO"?"_NN":""),
                                            finalName,
                                            Workspace::getInstance()->currentProjectsEngine()->getTypeStringForSqlType(type),
                                            type, size, defaultValue, "", QStringList(), false, type + " " + size,
                                            QString::compare(nullable, "YES", Qt::CaseInsensitive) == 0, false);

    newUdt->setName(NameGenerator::generateUniqueDatatypeName(this, newUdt->getName()));
    addNewDataType(newUdt);
    getGui()->createDataTypeTreeEntry(newUdt);

    return newUdt;
}
