#include "VersionUpdateGenerator.h"
#include "Version.h"
#include "core_Table.h"
#include "UidWarehouse.h"
#include "TableUpdateGenerator.h"
#include "Workspace.h"
#include "db_AbstractSQLGenerator.h"
#include "db_DatabaseEngine.h"
#include "Project.h"
#include "core_TableInstance.h"
#include "Configuration.h"

VersionUpdateGenerator::VersionUpdateGenerator(Version *from, Version *to) : m_commands()
{

    if(from->getObjectUid() == to->getObjectUid()) return;

    updateTables(from, to);
}

void VersionUpdateGenerator::updateTables(Version* from, Version* to)
{
    // section: tables

    // check: see if there are new tables in version "to" which are not there in "from".
    // the new tables are being put in a list, and they will be created at the end of this
    // method sicne there might be columns from other tables which are referenced as foreign
    // key from the new table
    QStringList newTabUids;                                                     // contains the UIDs of tables that are therein to and not in from
    const QVector<Table*>& toTables = to->getTables();
    const QVector<Table*>& fromTables = from->getTables();
    QStringList generatedTabinstUids;                                           // contains the UIDS of TabInsts for which we generated SQL already

    for(int i=0; i<toTables.size(); i++)
    {
        // a new table in the "to" version if it has a nullUid as the source ...
        if(toTables[i]->getSourceUid() == nullUid)
        {
            newTabUids.append(toTables[i]->getObjectUid());
        }
        else
        {
            bool foundARelation = false;
            // or if there are no tables in the base version to which it relates to
            for(int j=0; j<fromTables.size(); j++)
            {
                if(UidWarehouse::instance().related(toTables[i], fromTables[j]))
                {
                    foundARelation = true;
                    break;
                }
            }

            if(!foundARelation)
            {
                newTabUids.append(toTables[i]->getObjectUid());
            }
        }
    }

    // task: create all the new tables if the project is NOT OOP
    QHash<QString,QString> fo = Configuration::instance().sqlGenerationOptions();
    fo["FKSposition"] = "OnlyInternal";
    fo["PKSposition"] = "AfterColumnsDeclaration";

    if(!to->getProject()->oopProject())
    {
        bool fkAllowed = true;
        for(int i=0; i<newTabUids.size(); i++)
        {
            Table* t = to->getTableWithUid(newTabUids[i]);

            QStringList ctcomms = to->getProject()->getEngine()->getSqlGenerator()->generateCreateTableSql(t, fo, t->getName(), 0);
            m_commands << ctcomms;

            if(fkAllowed)
            {
                fkAllowed =! to->getProject()->getEngine()->tableBlocksForeignKeyFunctionality(t);
            }

        }

        if(fkAllowed)
        {
            for(int i=0; i<newTabUids.size(); i++)
            {
                Table* t = to->getTableWithUid(newTabUids[i]);

                QStringList foreignKeyCommands = to->getProject()->getEngine()->getSqlGenerator()->generateAlterTableForForeignKeys(t, fo);
                m_commands << foreignKeyCommands;
            }
        }

    }
    else
    {
        // first run: generate SQL for the elements defined in this version
        {
        const QVector<TableInstance*> &toTinsts = to->getTableInstances();
        bool fkAllowed = true;

        for(int i=0; i<toTinsts.size(); i++)
        {
            if(toTinsts[i]->getSourceUid() == nullUid)
            {
                // find the table
                Table* t = toTinsts[i]->table();
                if(newTabUids.contains(t->getObjectUid()))
                {
                    QStringList sql = toTinsts[i]->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), fo, 0);
                    m_commands << sql;
                    generatedTabinstUids.append(toTinsts[i]->getObjectUid());
                    if(fkAllowed) fkAllowed = !to->getProject()->getEngine()->tableBlocksForeignKeyFunctionality(toTinsts[i]->table());
                }
            }
        }

        if(fkAllowed)
        {
            for(int i=0; i<toTinsts.size(); i++)
            {
                if(toTinsts[i]->getSourceUid() == nullUid)
                {
                    QStringList foreignKeyCommands = to->getProject()->getEngine()->getSqlGenerator()->generateAlterTableForForeignKeys(toTinsts[i]->table(), fo);
                    m_commands << foreignKeyCommands;
                }
            }
        }
        }

        // second run: generate SQLS for the elements not necessarily defined in this version but which are not the in the from
        {
        bool fkAllowed = true;
        const QVector<TableInstance*> &toTinsts = to->getTableInstances();

        for(int i=0; i<newTabUids.size(); i++)
        {
            Table* t = to->getTableWithUid(newTabUids[i]);
            for(int j=0; j<toTinsts.size(); j++)
            {
                if(toTinsts[j]->table()->getObjectUid() == newTabUids[i])
                {
                    if(!generatedTabinstUids.contains(toTinsts[j]->getObjectUid()))
                    {
                        QStringList sql = toTinsts[j]->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), fo, 0);
                        m_commands << sql;
                        if(fkAllowed) fkAllowed = !to->getProject()->getEngine()->tableBlocksForeignKeyFunctionality(t);
                    }
                }
            }
        }

        if(fkAllowed)
        {
            for(int i=0; i<newTabUids.size(); i++)
            {
                for(int j=0; j<toTinsts.size(); j++)
                {
                    if(toTinsts[j]->table()->getObjectUid() == newTabUids[i])
                    {
                        if(!generatedTabinstUids.contains(toTinsts[j]->getObjectUid()))
                        {
                            QStringList foreignKeyCommands = to->getProject()->getEngine()->getSqlGenerator()->generateAlterTableForForeignKeys(toTinsts[j]->table(), fo);
                            m_commands << foreignKeyCommands;
                        }

                    }
                }
            }
        }
        }
    }

    QStringList fkCommands;

    // check: find all the related tables and generate update scripts.
    // Fetch the new foreign key commands into a separate vector
    // Fetch the dropped foreign key commands into a separate vector
    QStringList tableChanges;
    for(int i=0; i<fromTables.size(); i++)
    {
        for(int j=0; j<toTables.size(); j++)
        {
            if(UidWarehouse::instance().related(fromTables[i], toTables[j]))
            {
                bool proceed = false;
                if(!to->getProject()->oopProject())
                {
                    proceed = true;
                }
                else
                {
                    const QVector<TableInstance*>& toTabInsts = to->getTableInstances();
                    for(int k=0; k<toTabInsts.size(); k++)
                    {
                        if(toTabInsts[k]->table()->getObjectUid() == toTables[j]->getObjectUid())
                        {
                            proceed = true;
                            break;
                        }
                    }
                }
                if(proceed)
                {
                    tableChanges << "\n-- Update " + fromTables[i]->getName() + " (" +from->getVersionText() + ")  to " + toTables[j]->getName() + " (" +to->getVersionText() + ")";
                    TableUpdateGenerator tud(fromTables[i], toTables[j], Workspace::getInstance()->currentProjectsEngine());
                    tableChanges << tud.commands();
                    fkCommands << tud.droppedFksCommands();
                    fkCommands << tud.newFksCommands();
                }
            }
        }
    }

    // task: the alter scripts
    m_commands << tableChanges;

    // task: drop all the foreign keys
    m_commands << fkCommands;

    // check: see if there are deleted tables in version "to" which are not there in "from"
    if(!to->getProject()->oopProject())
    {
        QStringList droppedTables;
        for(int i=0; i<fromTables.size(); i++)
        {
            bool foundDescendant = false;
            for(int j=0; j<toTables.size(); j++)
            {
                if(UidWarehouse::instance().related(fromTables[i], toTables[j]))
                {
                    foundDescendant = true;
                    break;
                }
            }
            if(!foundDescendant)
            {
                droppedTables.append(fromTables[i]->getObjectUid());
            }
        }

        for(int i=0; i<droppedTables.size(); i++)
        {
            Table* t = from->getTableWithUid(droppedTables[i]);
            m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropTable(t->getName());
        }
    }
    else
    {
        const QVector<TableInstance*>& fromTabInsts = from->getTableInstances();
        const QVector<TableInstance*>& toTabInsts = to->getTableInstances();
        QStringList droppedTables;
        for(int i=0; i<fromTabInsts.size(); i++)
        {
            bool foundDescendant = false;
            for(int j=0; j<toTabInsts.size(); j++)
            {
                if(UidWarehouse::instance().related(fromTabInsts[i], toTabInsts[j]))
                {
                    foundDescendant = true;
                    break;
                }
            }
            if(!foundDescendant)
            {
                droppedTables.append(fromTabInsts[i]->getObjectUid());
            }
        }

        for(int i=0; i<droppedTables.size(); i++)
        {
            TableInstance* t = from->getTableInstanceWithUid(droppedTables[i]);
            m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropTable(t->getName());
        }
    }
}
