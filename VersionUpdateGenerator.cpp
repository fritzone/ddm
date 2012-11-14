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
#include "core_Column.h"
#include "ForeignKey.h"

VersionUpdateGenerator::VersionUpdateGenerator(Version *from, Version *to) : m_commands(), m_tablesReferencedWithFkFromOtherTables()
{
    if(from->getObjectUid() == to->getObjectUid()) return;

    updateTables(from, to);

    if(m_tablesReferencedWithFkFromOtherTables.keys().size())
    {
        // drop all the foreign keys from other tables that reference the tables/columns in this map
        QVector<ForeignKey*> droppedFks;
        QStringList dropCommands;
        QStringList createCommands;

        dropCommands << "-- Dropping foreign keys due to a change in the primary key structure of some tables";
        const QVector<Table*>& toTables = to->getTables();
        for(int i=0; i<toTables.size(); i++)
        {
            const QVector<ForeignKey*>& ofks = toTables.at(i)->getForeignKeys();
            for(int j=0; j<ofks.size(); j++)
            {
                ForeignKey* fkj = ofks.at(j);
                const QVector<ForeignKey::ColumnAssociation*>& ofkAssocs = fkj->getAssociations();
                for(int k=0; k<ofkAssocs.size(); k++)
                {
                    QString otherSLocalTable = ofkAssocs.at(k)->getSLocalTable();
                    QString otherSForeignTable = ofkAssocs.at(k)->getSForeignTable();
                    if(m_tablesReferencedWithFkFromOtherTables.contains(otherSForeignTable))
                    {
                        if(!to->getProject()->getEngine()->tableBlocksForeignKeyFunctionality(toTables[i]))
                        {
                            QStringList c = to->getProject()->getEngine()->getSqlGenerator()->getAlterTableForDropForeignKey(otherSLocalTable, fkj);
                            dropCommands << c;
                            droppedFks.append(fkj);
                        }
                    }
                }
            }
        }

        // then recreate them
        if(droppedFks.size())
        {
            for(int i=0; i<droppedFks.size(); i++)
            {
                ForeignKey* fkI = droppedFks.at(i);
                // TODO: Duplicate from the TableUpdateGenerator, MysqlCodeGenerator and it is MySQL specific
                {
                    // just pre-render the SQL for foreign keys
                    QString foreignKeySql1 = "";
                    QString foreignKeySql2 = "";

                    QString foreignKeysTable = fkI->getForeignTableName();
                    for(int j=0; j<fkI->getAssociations().size(); j++)
                    {

                        ForeignKey::ColumnAssociation* assocJ = fkI->getAssociations().at(j);
                        foreignKeySql1 += assocJ->getLocalColumn()->getName();
                        foreignKeySql2 += assocJ->getForeignColumn()->getName();

                        if(j < fkI->getAssociations().size() - 1)
                        {
                            foreignKeySql1 += ", ";
                            foreignKeySql2 += ", ";
                        }
                    }
                    QString foreignKeySql = " CONSTRAINT " + fkI->getName() + " FOREIGN KEY (";
                    foreignKeySql += foreignKeySql1;
                    foreignKeySql += ") REFERENCES ";
                    foreignKeySql += foreignKeysTable;
                    foreignKeySql += "(" + foreignKeySql2 + ")";
                    QString t = fkI->getOnDelete();
                    if(t.length() > 0) foreignKeySql += QString(" ") + ("ON DELETE ") + (t);
                    t = fkI->getOnUpdate();
                    if(t.length() > 0) foreignKeySql += QString(" ") + ("ON UPDATE ") + (t);

                    // TODO: This is just MySQL for now
                    QString f = "ALTER TABLE ";
                    f += fkI->getLocalTable()->getName();
                    f += " ADD ";
                    f += foreignKeySql;
                    createCommands << f;
                }
            }
            QStringList tCommands = dropCommands;
            tCommands << m_commands;
            tCommands << createCommands;

            m_commands = tCommands;
        }
    }

    updateTableInstances(from, to);

}

void VersionUpdateGenerator::updateTables(Version* from, Version* to)
{
    // section: tables

    // check: see if there are new tables in version "to" which are not there in "from".
    // the new tables are being put in a list, and they will be created at the end of this
    // method sicne there might be columns from other tables which are referenced as foreign
    // key from the new table
    QStringList newTabUids;                                                     // contains the UIDs of tables that are there in "to" and not in "from"
    const QVector<Table*>& toTables = to->getTables();
    const QVector<Table*>& fromTables = from->getTables();

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
                    m_generatedTabinstUids.append(toTinsts[i]->getObjectUid());
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
                    if(!m_generatedTabinstUids.contains(toTinsts[j]->getObjectUid()))
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
                        if(!m_generatedTabinstUids.contains(toTinsts[j]->getObjectUid()))
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
                    TableUpdateGenerator* tud = new TableUpdateGenerator(fromTables[i], toTables[j], Workspace::getInstance()->currentProjectsEngine());
                    m_tableUpdates[toTables[j]->getObjectUid()] = tud;
                    tableChanges << tud->commands();
                    fkCommands << tud->droppedFksCommands();
                    fkCommands << tud->newFksCommands();

                    if(tud->fkDroppingIsRequired())
                    {
                        m_tablesReferencedWithFkFromOtherTables.insert(toTables[j]->getName(), tud->getAffectedPkColumns());
                    }
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

void VersionUpdateGenerator::updateTableInstances(Version *from, Version *to)
{
    const QVector<TableInstance*>& toTabInsts = to->getTableInstances();
    const QVector<TableInstance*>& fromTabInsts = from->getTableInstances();

    for(int l=0; l<toTabInsts.size(); l++)
    {
        TableInstance* tinst = toTabInsts[l];
        if(!tinst)
        {
            continue;
        }

        // first run: the changed values in existing rows
        TableInstance* ancestor = 0;
        for(int j=0; j<fromTabInsts.size(); j++)
        {
            if(UidWarehouse::instance().related(fromTabInsts[j], tinst))
            {
                ancestor = fromTabInsts[j];
                break;
            }
        }
        if(ancestor == 0)
        {
            continue;
        }

        // the value comparison is done based on the primary keys, so it is not searching for primary key value changes
        // find all the primary keys in the from ancestor table
        QVector<Column*>fromPkColumns;
        QStringList fromCols = ancestor->table()->fullColumns();
        for(int i=0; i<fromCols.size(); i++)
        {
            Column* c = ancestor->table()->getColumn(fromCols[i]);
            if(!c) c = ancestor->table()->getColumnFromParents(fromCols[i]);
            if(!c) continue;

            if(c->isPk()) fromPkColumns.append(c);
        }
        // find all the primary keys in the from current table
        QVector<Column*>toPkColumns;
        QStringList toCols = tinst->table()->fullColumns();
        for(int i=0; i<toCols.size(); i++)
        {
            Column* c = tinst->table()->getColumn(toCols[i]);
            if(!c) c = tinst->table()->getColumnFromParents(toCols[i]);
            if(!c) continue;

            if(c->isPk()) toPkColumns.append(c);
        }
        // now build up a vector of PK values like: pk1_pk2_pk3_
        QHash < QString, QVector<QString> > fromVals = ancestor->values();
        QStringList fromTabInstPks;
        QVector<QString> cols = ancestor->columns();

        bool canGo = true;
        int trc = 0;

        while(canGo)
        {
            QString cRow = "";
            for(int i=0; i<fromPkColumns.size(); i++)
            {
                QVector<QString> cv = fromVals[fromPkColumns[i]->getName()];
                if(cv.size() > trc)
                {
                    cRow += cv[trc];
                }
                else
                {
                    canGo = false;
                }
                if(i<fromPkColumns.size() - 1) cRow += "_";
            }

            fromTabInstPks.append(cRow);
            trc ++;
        }
        qDebug() << fromTabInstPks;

        // second run: the new rows in the tinst

        // third run: adding the new columns
        TableUpdateGenerator* tud = m_tableUpdates[tinst->table()->getObjectUid()];
        if(!tud)
        {
            continue;
        }

        QVector<NewColumns> newColsForTab = tud->getNewColumns();
    }
}
