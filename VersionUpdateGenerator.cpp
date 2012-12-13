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
#include "core_View.h"
#include "core_Procedure.h"
#include "core_Function.h"
#include "strings.h"
#include "core_Trigger.h"

struct IndexHolder
{
    int allFromC, allToC, pksFromC, pksToC;
};

VersionUpdateGenerator::VersionUpdateGenerator(Version *from, Version *to) : m_commands(), m_tablesReferencedWithFkFromOtherTables(), m_delayedCommands()
{
    if(from->getObjectUid() == to->getObjectUid()) return;

    //m_commands << "-- Update script for version " + to->getVersionText() + " from " + from->getVersionText();
    //m_commands << "\n-- Tables";

    updateTables(from, to);

    if(m_tablesReferencedWithFkFromOtherTables.keys().size())
    {
        // drop all the foreign keys from other tables that reference the tables/columns in this map
        QVector<ForeignKey*> droppedFks;
        QStringList dropCommands;
        QStringList createCommands;

//        dropCommands << "-- Dropping foreign keys due to a change in the primary key structure of some tables";
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

    if(to->getProject()->oopProject())
    {
        updateTableInstances(from, to);
    }

    updateViews(from, to);

    // procedures, methods and triggers need to be dropped and recreated (at least in MySQL)

    updateProcedures(from, to);
    updateFunctions(from, to);
    updateTriggers(from, to);

}

void VersionUpdateGenerator::updateTables(Version* from, Version* to)
{
    // section: tables

    // first step: find the TableInstances which were renamed
    QVector<OldNameNewName*> renamedTableInstances;

    if(to->getProject()->oopProject())
    {
        const QVector<TableInstance*>& toTableInstances = to->getTableInstances();
        const QVector<TableInstance*>& fromTableInstances = from->getTableInstances();

        for(int i=0; i<fromTableInstances.size(); i++)
        {
            TableInstance* tableInstanceFrom = fromTableInstances.at(i);
            for(int j=0; j<toTableInstances.size(); j++)
            {
                TableInstance* tableInstanceTo = toTableInstances.at(j);
                if(UidWarehouse::instance().related(tableInstanceFrom, tableInstanceTo))
                {
                    if(tableInstanceTo->getName() != tableInstanceFrom->getName())
                    {
                        OldNameNewName* onnn = new OldNameNewName;
                        onnn->oldName = tableInstanceFrom->getName();
                        onnn->newName = tableInstanceTo->getName();
                        renamedTableInstances.append(onnn);
                    }
                }
            }
        }
    }

    // generate rename scripts for the renamed table instances
    for(int i=0; i<renamedTableInstances.size(); i++)
    {
        m_commands << to->getProject()->getEngine()->getSqlGenerator()->getTableRenameSql(renamedTableInstances[i]->oldName, renamedTableInstances[i]->newName);
    }
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
                else    // a new instance of an existing table... basically the same code as above.
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
        // ie. tables created in a version between current and previous, which were also instantiated
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
                    TableUpdateGenerator* tud = new TableUpdateGenerator(fromTables[i], toTables[j], Workspace::getInstance()->currentProjectsEngine(), this);
                    m_tableUpdates[toTables[j]->getObjectUid()] = tud;
                    tableChanges << tud->commands();
                    fkCommands << tud->droppedFksCommands();
                    fkCommands << tud->newFksCommands();
                    if(!to->getProject()->oopProject())
                    {
                        tableChanges << m_delayedCommands;
                    }

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
    // check for the dropped table instances and the new table instances
    {
        updateDifferentTableInstances(from, to);
    }
    // next run: the rows which have changed some values
    const QVector<TableInstance*> toTabInsts = to->getTableInstances();
    const QVector<TableInstance*> fromTabInsts = from->getTableInstances();

    for(int l=0; l<toTabInsts.size(); l++)
    {
        TableInstance* tinst = toTabInsts[l];
        if(!tinst)
        {
            continue;
        }

        // first run: the changed values in existing rows. Let's assume the PKs did not change
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
        QSet<Column*>fromPkColumns = ancestor->table()->primaryKeyColumns();
        // find all the primary keys in the from current table
        QSet<Column*>toPkColumns = tinst->table()->primaryKeyColumns();

        // Find the intersection of the two sets
        QSet<FromToColumn*> commonPks;
        foreach(Column* cFrom, fromPkColumns)
        {
            foreach(Column* cTo, toPkColumns)
            {
                if(UidWarehouse::instance().related(cFrom, cTo))
                {
                    FromToColumn* a = new FromToColumn;
                    a->from = cFrom;
                    a->to = cTo;
                    commonPks.insert(a);
                }
            }
        }
        // now the commonPKs has the set of common primary keys. Build up a vector of PK value maps to -> list of data values
        QVector<ColumnWithValue*> columnsWithValuesOfTo;
        QVector<ColumnWithValue*> columnsWithValuesOfFrom;

        foreach(FromToColumn* ftpk, commonPks)
        {
            ColumnWithValue* cwv = new ColumnWithValue;
            cwv->column = ftpk->from;
            columnsWithValuesOfFrom.append(cwv);

            ColumnWithValue* cwv1 = new ColumnWithValue;
            cwv1->column = ftpk->to;
            columnsWithValuesOfTo.append(cwv1);
        }

        // holds the values of the primary columns
        QVector <QVector<ColumnWithValue*> > pksTo = tinst->getValues(columnsWithValuesOfTo);
        QVector <QVector<ColumnWithValue*> > pksFrom = ancestor->getValues(columnsWithValuesOfFrom);

        // now get all the values from the tinst and ancestor table instances into allTo, allFrom variables
        QVector <QVector<ColumnWithValue*> > allTo = tinst->getFullValues();
        QVector <QVector<ColumnWithValue*> > allFrom = ancestor->getFullValues();

        generateDefaultValuesUpdateData(pksTo, pksFrom, allTo, allFrom, to, tinst->table()->getName(), false);

    }
}

void VersionUpdateGenerator::updateViews(Version *from, Version *to)
{
    const QVector<View*>& toViews = to->getViews();
    const QVector<View*>& fromViews = from->getViews();

    QSet<View*> foundToViews;
    QSet<View*> foundFromViews;
    // first step: find the views which have changed their sql and generate an alter statement
    for(int i=0; i<fromViews.size(); i++)
    {
        View* viewFrom = fromViews.at(i);
        for(int j=0; j<toViews.size(); j++)
        {
            View* viewTo = toViews.at(j);
            if(UidWarehouse::instance().related(viewFrom, viewTo))
            {
                foundToViews.insert(viewTo);
                foundFromViews.insert(viewFrom);

                if(viewFrom->getName() != viewTo->getName())
                {
                    // the view changed its name.
                    m_commands << to->getProject()->getEngine()->getSqlGenerator()->getTableRenameSql(viewFrom->getName(), viewTo->getName());
                }

                QString fromHash = viewFrom->getSqlHash();
                QString toHash = viewTo->getSqlHash();

                if(fromHash != toHash)
                {
                    // there is a change in the views' sql too.

                    // the creation step was changed. We alter it
                    QHash<QString, QString> opts = Configuration::instance().sqlOpts();
                    QStringList createViewLst = viewTo->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0);
                    QString createView = createViewLst.at(0);
                    // now chop the CREATE, put in the ALTER... sort of hack
                    int idx = 0;
                    QString cr = "";
                    while(createView.at(idx).isSpace()) idx ++;
                    while(createView.at(idx).isLetter())
                    {
                        cr += createView.at(idx);
                        idx ++;
                    }

                    // TODO: This is MYSQL spceific
                    if(cr.toUpper() == "CREATE")
                    {
                        m_commands << "ALTER " <<createView.mid(idx);
                    }

                }
            }
        }
    }

    // second run: drop the views that are not there in the second version
    for(int i=0; i<fromViews.size(); i++)
    {
        if(!foundFromViews.contains(fromViews[i]))
        {
            m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropView(fromViews[i]->getName());
        }
    }

    // third run the new views in "to"
    for(int i=0; i<toViews.size(); i++)
    {
        if(!foundToViews.contains(toViews[i]))
        {
            QHash<QString, QString> opts = Configuration::instance().sqlOpts();
            QStringList createViewLst = toViews[i]->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0);
            m_commands << createViewLst;
        }
    }

}

void VersionUpdateGenerator::generateDefaultValuesUpdateData(const QVector <QVector<ColumnWithValue*> >& pksTo,
                                                             const QVector <QVector<ColumnWithValue*> >& pksFrom,
                                                             const QVector <QVector<ColumnWithValue*> >& allTo,
                                                             const QVector <QVector<ColumnWithValue*> >& allFrom,
                                                             Version *to,
                                                             const QString& tabName, bool delayedCommands)
{
    if(delayedCommands)
    {
        m_delayedCommands.clear();
    }
    QVector<IndexHolder> fromToIndexMappings; // map to hold the corresponding indexes regarding the rows where the primary keys are equal

    // now walk through all the rows of the from and to tables
    for(int pksFromC = 0; pksFromC < pksFrom.size(); pksFromC ++)
    {
        for(int allFromC=0; allFromC<allFrom.size(); allFromC++)
        {
            QMap<QString, bool> founds;
            for(int j=0; j<pksFrom[pksFromC].size(); j++)
            {
                for(int i=0; i<allFrom[allFromC].size(); i++)
                {
                    if(founds.keys().contains(pksFrom[pksFromC][j]->column->getName()))
                    {
                        if(!founds[pksFrom[pksFromC][j]->column->getName()])
                        {
                            founds[pksFrom[pksFromC][j]->column->getName()] = false;
                        }
                    }
                    else
                    {
                        founds[pksFrom[pksFromC][j]->column->getName()] = false;
                    }
                    ColumnWithValue* cwv = allFrom[allFromC][i];
                    Column* c = cwv->column;
                    ColumnWithValue* pkcwv = pksFrom[pksFromC][j];
                    Column* pkC = pkcwv->column;
                    if(c->getName() == pkC->getName())
                    {
                        if(cwv->value == pkcwv->value)
                        {
                            founds[c->getName()] = true;
                        }
                    }
                }
            }

            // match the rows with their foreign key
            bool found = true;
            for(int j=0; j<founds.keys().size(); j++)
            {
                found &= founds[ founds.keys().at(j) ];
            }

            if(found)   // this means the allFromC row is the extension of the PK pksFromC row
            {
                // TODO: this loop is basically the same as above
                for(int pksToC = 0; pksToC < pksTo.size(); pksToC ++)
                {
                    for(int allToC=0; allToC<allTo.size(); allToC++)
                    {
                        QMap<QString, bool> foundsTo;
                        for(int j=0; j<pksTo[pksToC].size(); j++)
                        {
                            for(int i=0; i<allTo[allToC].size(); i++)
                            {
                                if(foundsTo.keys().contains(pksTo[pksToC][j]->column->getName()))
                                {
                                    if(!foundsTo[pksTo[pksToC][j]->column->getName()])
                                    {
                                        foundsTo[pksTo[pksToC][j]->column->getName()] = false;
                                    }
                                }
                                else
                                {
                                    foundsTo[pksTo[pksToC][j]->column->getName()] = false;
                                }
                                ColumnWithValue* cwv = allTo[allToC][i];
                                Column* c = cwv->column;
                                ColumnWithValue* pkcwv = pksTo[pksToC][j];
                                Column* pkC = pkcwv->column;
                                if(c->getName() == pkC->getName())
                                {
                                    if(cwv->value == pkcwv->value)
                                    {
                                        foundsTo[c->getName()] = true;
                                    }
                                }
                            }
                        }
                        bool foundTo = true;

                        for(int j=0; j<foundsTo.keys().size(); j++)
                        {
                            foundTo &= foundsTo[ foundsTo.keys().at(j) ];
                        }

                        if(foundTo)
                        {
                            // found another mathcing pair set from the "TO" side where pksTo[pksToC] is the key part of allTo[allToC]
                            // See if the primary keys match or not... ie:
                            // all the values of pksFrom[pksFromC] should map pksTo[pksToC]. The order is already predefined, we just compare the values
                            QString froms;
                            for(int j=0; j<pksFrom[pksFromC].size(); j++)
                            {
                                froms += pksFrom[pksFromC][j]->value + " ";
                            }
                            QString tos;
                            for(int j=0; j<pksTo[pksToC].size(); j++)
                            {
                                tos += pksTo[pksToC][j]->value + " ";
                            }
                            froms = froms.trimmed();
                            tos = tos.trimmed();

                            // and set the corresponding values
                            if(froms == tos)
                            {
                                IndexHolder ih;
                                ih.allFromC = allFromC;
                                ih.allToC = allToC;
                                ih.pksFromC = pksFromC;
                                ih.pksToC = pksToC;
                                fromToIndexMappings.append(ih);
                            }
                        }
                    }
                }
            }
        }
    }

    // now in the vector above we have the matching rows of the allTo and allFrom vectors ... see if the rows match or not
    for(int keyCounter=0; keyCounter<fromToIndexMappings.size(); keyCounter++)
    {
        int idxFrom = fromToIndexMappings[keyCounter].allFromC;
        int idxTo = fromToIndexMappings[keyCounter].allToC;

        const QVector<ColumnWithValue*>& fromRow = allFrom[idxFrom];
        const QVector<ColumnWithValue*>& toRow = allTo[idxTo];

        QSet<Column*> usedColumns;   // holds all the columns for which a descendant was found in this row. The other columns are new columns, will be handled after
        for(int i=0; i<fromRow.size(); i++)
        {
            int idxFound = -1;
            // now find the corresponding column in "to"
            for(int j=0; j<toRow.size(); j++)
            {
                if(UidWarehouse::instance().related(fromRow[i]->column, toRow[j]->column))
                {
                    usedColumns.insert(toRow[j]->column);
                    idxFound = j;
                    break;
                }
            }

            // and see if we have found something or not
            if(idxFound != -1)
            {
                if(fromRow[i]->value != toRow[idxFound]->value)
                {
                    // now generate an update script
                    int pksToIdx = fromToIndexMappings[keyCounter].pksToC;
                    QStringList pksToNames;
                    QStringList pksToValues;
                    for(int k = 0; k<pksTo[pksToIdx].size(); k++)
                    {
                        pksToNames << pksTo[pksToIdx][k]->column->getName();
                        pksToValues <<pksTo[pksToIdx][k]->value;
                    }
                    QString command = to->getProject()->getEngine()->getSqlGenerator()->getUpdateTableForColumns(tabName, pksToNames, pksToValues, toRow[idxFound]->column->getName(), toRow[idxFound]->value);
                    if(!delayedCommands) m_commands << command;
                    else m_delayedCommands << command;
                }
            }
        }

        QSet<Column*> allColumns;
        for(int i=0; i<toRow.size(); i++)
        {
            allColumns.insert(toRow[i]->column);
        }

        QSet<Column*>  newColumnsInTo = allColumns - usedColumns;
        foreach(Column* col, newColumnsInTo)
        {
            int idx2Found = -1;
            for(int j=0; j<toRow.size(); j++)
            {
                if(toRow[j]->column == col)
                {
                    idx2Found = j;
                    break;
                }
            }


            int pksToIdx = fromToIndexMappings[keyCounter].pksToC;
            QStringList pksToNames;
            QStringList pksToValues;
            for(int k = 0; k<pksTo[pksToIdx].size(); k++)
            {
                pksToNames << pksTo[pksToIdx][k]->column->getName();
                pksToValues <<pksTo[pksToIdx][k]->value;
            }

            QString command = to->getProject()->getEngine()->getSqlGenerator()->getUpdateTableForColumns(tabName, pksToNames, pksToValues, col->getName(), toRow[idx2Found]->value);
            if(!delayedCommands) m_commands << command;
            else m_delayedCommands << command;
        }
    }

    // next run: the deleted rows. These will be picked out from the fromToIndexMappings: if the fromIndex is NOT in the vector this means that row was deleted
    {
    QSet<int> missingRows;
    QSet<int> foundRows;
    QSet<int> allRows;
    for(int keyCounter=0; keyCounter<fromToIndexMappings.size(); keyCounter++)
    {
        int idxFrom = fromToIndexMappings[keyCounter].pksFromC;
        foundRows.insert(idxFrom);
    }
    for(int i=0; i<pksFrom.size(); i++)
    {
        allRows.insert(i);
    }
    missingRows = allRows - foundRows;

    foreach(int idx, missingRows)
    {
        QStringList pksFromName;
        QStringList pksFromValues;
        for(int k = 0; k<pksFrom[idx].size(); k++)
        {
            pksFromName << pksFrom[idx][k]->column->getName();
            pksFromValues <<pksFrom[idx][k]->value;
        }
        QString command = to->getProject()->getEngine()->getSqlGenerator()->getDeleteFromTable(tabName, pksFromName, pksFromValues);
        if(!delayedCommands) m_commands << command;
        else m_delayedCommands << command;

    }
    }

    // next run: the new rows in the tinst: these will be picked up from the fromToIndexMappings: if the toIndex is NOT in it this is a new row
    {
    QSet<int> missingRows;
    QSet<int> foundRows;
    QSet<int> allRows;
    for(int keyCounter=0; keyCounter<fromToIndexMappings.size(); keyCounter++)
    {
        int idxTo = fromToIndexMappings[keyCounter].pksToC;
        foundRows.insert(idxTo);
    }
    for(int i=0; i<pksTo.size(); i++)
    {
        allRows.insert(i);
    }
    missingRows = allRows - foundRows;

    foreach(int idx, missingRows)
    {
        for(int allToC=0; allToC<allTo.size(); allToC++)
        {
            QMap<QString, bool> foundsTo;
            for(int j=0; j<pksTo[idx].size(); j++)
            {
                for(int i=0; i<allTo[allToC].size(); i++)
                {
                    if(foundsTo.keys().contains(pksTo[idx][j]->column->getName()))
                    {
                        if(!foundsTo[pksTo[idx][j]->column->getName()])
                        {
                            foundsTo[pksTo[idx][j]->column->getName()] = false;
                        }
                    }
                    else
                    {
                        foundsTo[pksTo[idx][j]->column->getName()] = false;
                    }
                    ColumnWithValue* cwv = allTo[allToC][i];
                    Column* c = cwv->column;
                    ColumnWithValue* pkcwv = pksTo[idx][j];
                    Column* pkC = pkcwv->column;
                    if(c->getName() == pkC->getName())
                    {
                        if(cwv->value == pkcwv->value)
                        {
                            foundsTo[c->getName()] = true;
                        }
                    }
                }
            }
            bool foundTo = true;

            for(int j=0; j<foundsTo.keys().size(); j++)
            {
                foundTo &= foundsTo[ foundsTo.keys().at(j) ];
            }

            if(foundTo)
            {
                QStringList cols;
                QStringList vals;
                for(int i=0; i<allTo[allToC].size(); i++)
                {
                    ColumnWithValue* cwv = allTo[allToC][i];
                    cols << cwv->column->getName();
                    vals << cwv->value;
                }
                QString command = to->getProject()->getEngine()->getSqlGenerator()->getInsertsIntoTable(tabName, cols, vals);
                if(!delayedCommands) m_commands << command;
                else m_delayedCommands << command;
            }
        }
    }
    }
}

void VersionUpdateGenerator::updateProcedures(Version *from, Version *to)
{
    const QVector<Procedure*>& toProcedures = to->getProcedures();
    const QVector<Procedure*>& fromProcedures = from->getProcedures();

    QSet<Procedure*> foundToProcedures;
    QSet<Procedure*> foundFromProcedures;
    // first step: find the Procedures which have changed their sql and generate an alter statement
    for(int i=0; i<fromProcedures.size(); i++)
    {
        Procedure* procedureFrom = fromProcedures.at(i);
        for(int j=0; j<toProcedures.size(); j++)
        {
            Procedure* procedureTo = toProcedures.at(j);
            if(UidWarehouse::instance().related(procedureFrom, procedureTo))
            {
                foundToProcedures.insert(procedureTo);
                foundFromProcedures.insert(procedureFrom);
                bool recreated = false;

                if(procedureFrom->getName() != procedureTo->getName())
                {
                    // the Procedure changed its name. Drop it and recreate it. There is no other generic way
                    QHash<QString, QString> opts = Configuration::instance().sqlOpts();
                    m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropProcedure(procedureFrom->getName());
                    m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
                    QStringList t = procedureTo->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;
                    QString s = "";
                    for(int j=0; j<t.size(); j++)
                    {
                        s += t.at(j) + " ";
                    }
                    s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
                    m_commands << s;
                    m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
                    recreated = true;
                }

                QString fromHash = procedureFrom->getSqlHash();
                QString toHash = procedureTo->getSqlHash();

                if(fromHash != toHash)
                {
                    if(!recreated)
                    {
                        QHash<QString, QString> opts = Configuration::instance().sqlOpts();
                        m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropProcedure(procedureFrom->getName());
                        m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
                        QStringList t = procedureTo->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;
                        QString s = "";
                        for(int j=0; j<t.size(); j++)
                        {
                            s += t.at(j) + " ";
                        }
                        s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
                        m_commands << s;
                        m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
                    }
                }
            }
        }
    }

    // second run: drop the Procedures that are not there in the second version
    for(int i=0; i<fromProcedures.size(); i++)
    {
        if(!foundFromProcedures.contains(fromProcedures[i]))
        {
            m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropProcedure(fromProcedures[i]->getName());
        }
    }

    // third run the new Procedures in "to"
    for(int i=0; i<toProcedures.size(); i++)
    {
        if(!foundToProcedures.contains(toProcedures[i]))
        {
            QHash<QString, QString> opts = Configuration::instance().sqlOpts();
            m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
            QStringList t = toProcedures[i]->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;
            QString s = "";
            for(int j=0; j<t.size(); j++)
            {
                s += t.at(j) + " ";
            }
            s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;

            m_commands << s;
            m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
        }
    }

}

void VersionUpdateGenerator::updateFunctions(Version *from, Version *to)
{
    const QVector<Function*>& toFunctions = to->getFunctions();
    const QVector<Function*>& fromFunctions = from->getFunctions();

    QSet<Function*> foundToFunctions;
    QSet<Function*> foundFromFunctions;
    // first step: find the Functions which have changed their sql and generate an alter statement
    for(int i=0; i<fromFunctions.size(); i++)
    {
        Function* FunctionFrom = fromFunctions.at(i);
        for(int j=0; j<toFunctions.size(); j++)
        {
            Function* FunctionTo = toFunctions.at(j);
            if(UidWarehouse::instance().related(FunctionFrom, FunctionTo))
            {
                foundToFunctions.insert(FunctionTo);
                foundFromFunctions.insert(FunctionFrom);
                bool recreated = false;

                if(FunctionFrom->getName() != FunctionTo->getName())
                {
                    // the Function changed its name. Drop it and recreate it. There is no other generic way
                    QHash<QString, QString> opts = Configuration::instance().sqlOpts();
                    m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropFunction(FunctionFrom->getName());
                    m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
                    QStringList t = FunctionTo->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;
                    QString s = "";
                    for(int j=0; j<t.size(); j++)
                    {
                        s += t.at(j) + " ";
                    }
                    s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
                    m_commands << s;
                    m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
                    recreated = true;
                }

                QString fromHash = FunctionFrom->getSqlHash();
                QString toHash = FunctionTo->getSqlHash();

                if(fromHash != toHash)
                {
                    if(!recreated)
                    {
                        QHash<QString, QString> opts = Configuration::instance().sqlOpts();
                        m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropFunction(FunctionFrom->getName());
                        m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
                        QStringList t = FunctionTo->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;
                        QString s = "";
                        for(int j=0; j<t.size(); j++)
                        {
                            s += t.at(j) + " ";
                        }
                        s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
                        m_commands << s;
                        m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
                    }
                }
            }
        }
    }

    // second run: drop the Functions that are not there in the second version
    for(int i=0; i<fromFunctions.size(); i++)
    {
        if(!foundFromFunctions.contains(fromFunctions[i]))
        {
            m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropFunction(fromFunctions[i]->getName());
        }
    }

    // third run the new Functions in "to"
    for(int i=0; i<toFunctions.size(); i++)
    {
        if(!foundToFunctions.contains(toFunctions[i]))
        {
            QHash<QString, QString> opts = Configuration::instance().sqlOpts();
            m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;
            QStringList t = toFunctions[i]->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;
            QString s = "";
            for(int j=0; j<t.size(); j++)
            {
                s += t.at(j) + " ";
            }
            s += Configuration::instance().sqlOpts()[strSqlDelimiterText] + strNewline;

            m_commands << s;
            m_commands << to->getProject()->getEngine()->getDelimiterKeyword() + strSpace + strSemicolon + strNewline;
        }
    }

}

void VersionUpdateGenerator::updateTriggers(Version *from, Version *to)
{
    const QVector<Trigger*>& toTriggers = to->getTriggers();
    const QVector<Trigger*>& fromTriggers = from->getTriggers();

    QSet<Trigger*> foundToTriggers;
    QSet<Trigger*> foundFromTriggers;
    // first step: find the Triggers which have changed their sql and generate an alter statement
    for(int i=0; i<fromTriggers.size(); i++)
    {
        Trigger* TriggerFrom = fromTriggers.at(i);
        for(int j=0; j<toTriggers.size(); j++)
        {
            Trigger* TriggerTo = toTriggers.at(j);
            if(UidWarehouse::instance().related(TriggerFrom, TriggerTo))
            {
                foundToTriggers.insert(TriggerTo);
                foundFromTriggers.insert(TriggerFrom);
                bool recreated = false;

                if(TriggerFrom->getName() != TriggerTo->getName())
                {
                    // the Trigger changed its name. Drop it and recreate it. There is no other generic way
                    QHash<QString, QString> opts = Configuration::instance().sqlOpts();
                    m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropTrigger(TriggerFrom->getName());
                    m_commands <<  TriggerTo->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;
                    recreated = true;
                }

                QString fromHash = TriggerFrom->getSqlHash();
                QString toHash = TriggerTo->getSqlHash();

                if(fromHash != toHash)
                {
                    if(!recreated)
                    {
                        QHash<QString, QString> opts = Configuration::instance().sqlOpts();
                        m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropTrigger(TriggerFrom->getName());
                        m_commands << TriggerTo->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;
                    }
                }
            }
        }
    }

    // second run: drop the Triggers that are not there in the second version
    for(int i=0; i<fromTriggers.size(); i++)
    {
        if(!foundFromTriggers.contains(fromTriggers[i]))
        {
            m_commands << to->getProject()->getEngine()->getSqlGenerator()->getDropTrigger(fromTriggers[i]->getName());
        }
    }

    // third run the new Triggers in "to"
    for(int i=0; i<toTriggers.size(); i++)
    {
        if(!foundToTriggers.contains(toTriggers[i]))
        {
            QHash<QString, QString> opts = Configuration::instance().sqlOpts();
            m_commands << toTriggers[i]->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), opts, 0) ;;
        }
    }

}

void VersionUpdateGenerator::updateDifferentTableInstances(Version *from, Version *to)
{
    const QVector<TableInstance*>& toTableInstances = to->getTableInstances();
    const QVector<TableInstance*>& fromTableInstances = from->getTableInstances();

    QSet<TableInstance*> foundToTableInstances;
    QSet<TableInstance*> foundFromTableInstances;
    // first step: find the TableInstances which have changed their sql and generate an alter statement
    for(int i=0; i<fromTableInstances.size(); i++)
    {
        TableInstance* tableInstanceFrom = fromTableInstances.at(i);
        for(int j=0; j<toTableInstances.size(); j++)
        {
            TableInstance* tableInstanceTo = toTableInstances.at(j);
            if(UidWarehouse::instance().related(tableInstanceFrom, tableInstanceTo))
            {
                foundToTableInstances.insert(tableInstanceTo);
                foundFromTableInstances.insert(tableInstanceFrom);
            }
        }
    }


    // generate the create table SQL script for the new table instances
    bool fkAllowed = true;
    QStringList newUids;
    QHash<QString,QString> fo = Configuration::instance().sqlGenerationOptions();
    fo["FKSposition"] = "OnlyInternal";
    fo["PKSposition"] = "AfterColumnsDeclaration";
    // third run the new TableInstances in "to"
    for(int i=0; i<toTableInstances.size(); i++)
    {
        if(!foundToTableInstances.contains(toTableInstances[i]))
        {
            if(!m_generatedTabinstUids.contains(toTableInstances[i]->getObjectUid()))
            {
                QStringList sql = toTableInstances[i]->generateSqlSource(to->getProject()->getEngine()->getSqlGenerator(), fo, 0);
                m_commands << sql;
                newUids.append(toTableInstances[i]->getObjectUid());
                if(fkAllowed) fkAllowed = !to->getProject()->getEngine()->tableBlocksForeignKeyFunctionality(toTableInstances[i]->table());
            }

        }
    }

    if(fkAllowed)
    {
        for(int j=0; j<newUids.size(); j++)
        {
            TableInstance* tinst = dynamic_cast<TableInstance*>(UidWarehouse::instance().getElement(newUids[j]));
            if(tinst && !m_generatedTabinstUids.contains(tinst->getObjectUid()))
            {
                QStringList foreignKeyCommands = to->getProject()->getEngine()->getSqlGenerator()->generateAlterTableForForeignKeys(tinst->table(), fo);
                m_commands << foreignKeyCommands;
            }
        }
    }


}
