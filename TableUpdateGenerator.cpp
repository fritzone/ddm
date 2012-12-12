#include "TableUpdateGenerator.h"
#include "core_Table.h"
#include "core_Column.h"
#include "UidWarehouse.h"
#include "db_AbstractSQLGenerator.h"
#include "db_DatabaseEngine.h"
#include "core_UserDataType.h"
#include "ForeignKey.h"
#include "Version.h"
#include "core_ColumnWithValue.h"
#include "VersionUpdateGenerator.h"

struct OldNameNewName
{
    QString oldName;
    QString newName;
};


struct ColumnOrderChange
{
    // the name of the column (from Table 2)
    QString name;

    // the column aftwer which this should be added (from Table 2)
    QString afterColumn;
};

TableUpdateGenerator::TableUpdateGenerator(Table *t1, Table *t2, DatabaseEngine* dbEngine, VersionUpdateGenerator* vug) : m_futurePksDropNeeded(false)
{
    // see if these two tables have a common ancestor or not
    bool related = UidWarehouse::instance().related(t1, t2);
    if(!related) return;

    // see if the second table has changed its name or not
    if(t2->getName() != t1->getName())
    {
//        m_commands.append("-- table " + t1->getName() + " renamed to: " + t2->getName());
        m_commands.append(dbEngine->getSqlGenerator()->getTableRenameSql(t1->getName(), t2->getName()));
    }

    QVector<OldNameNewName> renamedColumns;
    QVector<ColumnOrderChange> columnsThatHaveChangedOrder;
    QVector<QString> deletedColumns;
    QVector<Column*> changedColumns;    // contains a list of columns that have chagned their datatype
    QStringList droppedPrimaryKeys;     // contains the columns of T2 which were primary kesy in T1
    QStringList newPrimaryKeys;         // contains the primary key columns of T2 which wre NOT primary keys in T1

    // next run. See if T2 has new or renamed columns
    QStringList t2Columns = t2->fullColumns();
    for(int i=0; i<t2Columns.size(); i++)
    {
        if(!t1->hasColumn(t2Columns[i]))
        {
            if(!t1->parentsHaveColumn(t2Columns[i]))
            {
                // seems, there is a column which is either new in t2 or was renamed
                Column* ct2 = t2->getColumn(t2Columns[i]);
                if(!ct2) ct2 = t2->getColumnFromParents(t2Columns[i]);
                if(!ct2) continue;

                // is this column new in T2?
                if(ct2->getSourceUid() == nullUid)
                {
                    // this is a new column
                    NewColumns nc;
                    nc.newName = t2Columns[i];
                    if(i > 0) nc.afterColumn = t2Columns[i - 1];
                    m_newColumns.append(nc);

                    // is this a primary key?
                    if(ct2->isPk())
                    {
                        newPrimaryKeys.append(ct2->getName());
                    }
                }
                else
                {
                    // this is a renamed column
                    OldNameNewName nn;
                    nn.newName = t2Columns[i];
                    // now find the column which has a different name for a column with objectUid = ct2.sourceUid
                    bool found = false;
                    Column *c = ct2;
                    QString oldName = "";
                    while(!found)
                    {
                        QString srcUid = c->getSourceUid();
                        if(srcUid == nullUid) break;
                        c = dynamic_cast<Column*>(UidWarehouse::instance().getElement(srcUid));
                        if(!c) break;
                        if(c->getName() != ct2->getName())
                        {
                            oldName = c->getName();
                            found = true;
                            break;
                        }
                    }
                    nn.oldName = oldName;
                    renamedColumns.append(nn);

                    // is this a primary key? If yes later the foreign keys need to be recreated
                    if(ct2->isPk())
                    {
                        newPrimaryKeys.append(ct2->getName());
                        m_pkRenamedColsAffected.append(ct2->getName());
                        m_futurePksDropNeeded = true;
                    }
                }
            }
        }
        else
        {
            // the column is there, see if the index of it was changed or not.
            int cidx = t1->getIndexOfColumn(t2Columns[i]);

            // also see that the data type is the same
            Column* c1 = t1->getColumn(t2Columns[i]);
            if(!c1) c1 = t1->getColumnFromParents(t2Columns[i]);
            if(!c1) continue;

            Column* c2 = t2->getColumn(t2Columns[i]);
            if(!c2) c2 = t2->getColumnFromParents(t2Columns[i]);
            if(!c2) continue;

            if(c1->getDataType()->sqlAsString() != c2->getDataType()->sqlAsString())
            {
                changedColumns.append(c2);
            }

            // see if the c1 was primary key and if c2 is NOT add to the list
            if(c1->isPk())
            {
                if(!c2->isPk())
                {
                    droppedPrimaryKeys.append(c2->getName());
                    // here we don't need to recreate the foreign keys, the GUI forced us to drop them before
                }
            }

            // and the other way around
            if(c2->isPk())
            {
                if(!c1->isPk())
                {
                    newPrimaryKeys.append(c2->getName());
                }
            }

            if(cidx == i)
            {
                // column is at the same location
                continue;
            }
            // however only if there are no new columns before it.

            if(cidx >= t2Columns.size()) cidx = t2Columns.size() - 1;

            bool newColumnsBefore = false;
            for(int j = cidx; j> -1; j--)
            {
                Column* ct2inner = t2->getColumn(t2Columns[j]);
                if(!ct2inner) ct2inner = t2->getColumnFromParents(t2Columns[j]);
                if(!ct2inner) continue;
                if(ct2inner->getSourceUid() == nullUid)
                {
                    newColumnsBefore = true;
                    // this is a new column in this table
                    break;
                }
            }

            if(!newColumnsBefore)
            {
                ColumnOrderChange cc;
                cc.name = t2Columns[i];
                if(cidx > 0) cc.afterColumn = t2Columns[i - 1];

                columnsThatHaveChangedOrder.append(cc);
            }
        }
    }

    // next run: see if there were any columns deleted in t2
    QStringList t1Columns = t1->fullColumns();
    for(int i=0; i<t1Columns.size(); i++)
    {
        Column* c1 = t1->getColumn(t1Columns[i]);
        if(!c1) c1 = t1->getColumnFromParents(t1Columns[i]);
        if(!c1) continue;
        bool columnFoundSomewhere = false;
        int tidx = t2Columns.indexOf(c1->getName()) ;

        if(tidx == -1)
        {
            // the column is not there in the names, we might need to see if it was renamed or not through a generation of tables
            for(int j=0; j<t2Columns.size(); j++)
            {
                Column* ct2 = t2->getColumn(t2Columns[j]);
                if(!ct2) ct2 = t2->getColumnFromParents(t2Columns[j]);
                if(!ct2) continue;
                // now see that after some time the ancestry of ct2 is the same as ct1
                if(UidWarehouse::instance().related(c1, ct2))
                {
                    columnFoundSomewhere = true;
                    break;
                }
            }
        }
        else
        {
            columnFoundSomewhere = true;
        }

        if(!columnFoundSomewhere)
        {
            deletedColumns.append(c1->getName());
        }
    }

    // renamed columns should be the first. There might be new columns after a renamed one
    if(renamedColumns.size()) m_commands.append("\n");
    for(int i=0; i< renamedColumns.size() ; i++)
    {
        Column* ct2 = t2->getColumn(renamedColumns[i].newName);
        if(!ct2) ct2 = t2->getColumnFromParents(renamedColumns[i].newName);
        if(!ct2) continue;

//        m_commands.append("-- rename column " + renamedColumns[i].oldName + " to " + renamedColumns[i].newName);
        m_commands.append(dbEngine->getSqlGenerator()->getAlterTableForColumnRename(t2->getName(), ct2, renamedColumns[i].oldName));

    }

    if(m_newColumns.size()) m_commands.append("\n");
    for(int i=0; i< m_newColumns.size() ; i++)
    {
        Column* ct2 = t2->getColumn(m_newColumns[i].newName);
        if(!ct2) ct2 = t2->getColumnFromParents(m_newColumns[i].newName);
        if(!ct2) continue;

//        m_commands.append("-- new column " + m_newColumns[i].newName);
        m_commands.append(dbEngine->getSqlGenerator()->getAlterTableForNewColumn(t2->getName(), ct2, m_newColumns[i].afterColumn));
    }

    if(m_newColumns.size() == 0 && deletedColumns.size() == 0)
    {
        // only create the change order columns if there are no new columns
        if(columnsThatHaveChangedOrder.size()) m_commands.append("\n");
        for(int i=0; i<columnsThatHaveChangedOrder.size(); i++)
        {
            Column* ct2 = t2->getColumn(columnsThatHaveChangedOrder[i].name);
            if(!ct2) ct2 = t2->getColumnFromParents(columnsThatHaveChangedOrder[i].name);
            if(!ct2) continue;

//            m_commands.append("-- column " + columnsThatHaveChangedOrder[i].name + " was relocated " +
//                              (columnsThatHaveChangedOrder[i].afterColumn.isEmpty()? " to the first location "
//                                                                                  : "after " + columnsThatHaveChangedOrder[i].afterColumn));
            m_commands.append(dbEngine->getSqlGenerator()->getAlterTableForChangeColumnOrder(t2->getName(), ct2, columnsThatHaveChangedOrder[i].afterColumn));
        }
    }

    if(deletedColumns.size()) m_commands.append("\n");
    for(int i=0; i<deletedColumns.size(); i++)
    {
//        m_commands.append("-- column " + deletedColumns[i] + " was deleted");
        m_commands.append(dbEngine->getSqlGenerator()->getAlterTableForColumnDeletion(t2->getName(), deletedColumns[i]));
    }

    if(changedColumns.size()) m_commands.append("\n");
    for(int i=0; i<changedColumns.size(); i++)
    {
//        m_commands.append("-- column " + changedColumns[i]->getName() + " has changed its datatype");
        m_commands.append(dbEngine->getSqlGenerator()->getAlterTableForColumnChange(t2->getName(), changedColumns[i]));
    }

    // now find all the dropped foreign keys (ie: foreign keys that are there in T1 and not in T2)
    QVector<ForeignKey*> droppedFksFromT2;
    const QVector<ForeignKey*>& fksOfT1 = t1->getForeignKeys();
    for(int i=0; i<fksOfT1.size(); i++)
    {
        bool foundARelatedFk = false;
        const QVector<ForeignKey*>& fksOfT2 = t2->getForeignKeys();
        for(int j=0; j<fksOfT2.size(); j++)
        {
            if(UidWarehouse::instance().related(fksOfT1[i], fksOfT2[j]))
            {
                foundARelatedFk = true;
                break;
            }
        }
        if(!foundARelatedFk)
        {
            droppedFksFromT2.append(fksOfT1[i]);
        }
    }

    // and generate the required SQL commands for dropping a foreign key
    for(int i=0; i<droppedFksFromT2.size(); i++)
    {
//        m_droppedForeignKeys.append("-- foreign key " + droppedFksFromT2[i]->getName() + " was dropped");
        m_droppedForeignKeys << dbEngine->getSqlGenerator()->getAlterTableForDropForeignKey(t2->getName(), droppedFksFromT2[i]);
    }

    // now search for NEW foreign keys
    QStringList newFksFromT2;
    const QVector<ForeignKey*>& fksOfT2 = t2->getForeignKeys();
    for(int i=0; i<fksOfT2.size(); i++)
    {
        bool foundARelatedFk = false;
        const QVector<ForeignKey*>& fksOfT1 = t1->getForeignKeys();
        for(int j=0; j<fksOfT1.size(); j++)
        {
            if(UidWarehouse::instance().related(fksOfT2[i], fksOfT1[j]))
            {
                foundARelatedFk = true;
                break;
            }
        }
        if(!foundARelatedFk)
        {
            newFksFromT2.append(fksOfT2[i]->getName());
        }
    }
    for(int k=0; k<newFksFromT2.size(); k++)
    {
//        m_newForeignKeys.append("-- foreign key " + newFksFromT2[k] + " was created");
        // creating the FOREIGN KEY sql(s)...
        for(int i=0; i<t2->getForeignKeys().size(); i++)
        {
            ForeignKey* fkI = t2->getForeignKeys().at(i);

            if(newFksFromT2.contains(fkI->getName()))
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
                f += t2->getName();
                f += " ADD ";
                f += foreignKeySql;
                m_newForeignKeys.append(f);
            }
        }
    }

    // and now see if the project is NOT oop that the default values are updated too
    if(!vug) return;
    if(!t2->version()->getProject()->oopProject())
    {
        // algorithm similar to tableInstance update from VersionUpdateGenerator
        QSet<Column*>fromPkColumns = t1->primaryKeyColumns();
        QSet<Column*>toPkColumns = t2->primaryKeyColumns();

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
        QVector <QVector<ColumnWithValue*> > pksTo = t2->getValues(columnsWithValuesOfTo);
        QVector <QVector<ColumnWithValue*> > pksFrom = t1->getValues(columnsWithValuesOfFrom);

        // now get all the values from the tinst and ancestor table instances into allTo, allFrom variables
        QVector <QVector<ColumnWithValue*> > allTo = t2->getFullValues();
        QVector <QVector<ColumnWithValue*> > allFrom = t1->getFullValues();

        vug->generateDefaultValuesUpdateData(pksTo, pksFrom, allTo, allFrom, t2->version(), t2->getName(), true);
    }
}
