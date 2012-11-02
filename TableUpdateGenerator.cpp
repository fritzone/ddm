#include "TableUpdateGenerator.h"
#include "core_Table.h"
#include "core_Column.h"
#include "UidWarehouse.h"
#include "db_DatabaseEngine.h"

struct OldNameNewName
{
    QString oldName;
    QString newName;
};

struct NewColumns
{
    // the new name of the column (from Table 2)
    QString newName;

    // the column aftwer which this should be added (from Table 2)
    QString afterColumn;
};

struct ColumnOrderChange
{
    // the name of the column (from Table 2)
    QString name;

    // the column aftwer which this should be added (from Table 2)
    QString afterColumn;
};

TableUpdateGenerator::TableUpdateGenerator(Table *t1, Table *t2, DatabaseEngine* dbEngine)
{
    // see if these two tables have a common ancestor or not
    bool related = UidWarehouse::instance().related(t1, t2);
    if(!related) return;

    // see if the second table has changed its name or not
    if(t2->getName() != t1->getName())
    {
        m_commands.append("-- table " + t1->getName() + " renamed to: " + t2->getName());
        m_commands.append(dbEngine->getTableRenameSql(t1->getName(), t2->getName()));
    }

    QVector<NewColumns> newColumns;     // contains a lis of columns that are new in T2
    QVector<OldNameNewName> renamedColumns;

    QVector<ColumnOrderChange> columnsThatHaveChangedOrder;

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
                    newColumns.append(nc);
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
                }
            }
        }
        else
        {
            // the column is there, see if the index of it was changed or not.
            int cidx = t1->getIndexOfColumn(t2Columns[i]);

            if(cidx == i)
            {
                // column is at the same location
                continue;
            }
            // however only if there are no new columns before it.

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

    if(newColumns.size()) m_commands.append("\n");
    for(int i=0; i< newColumns.size() ; i++)
    {
        Column* ct2 = t2->getColumn(newColumns[i].newName);
        if(!ct2) ct2 = t2->getColumnFromParents(newColumns[i].newName);
        if(!ct2) continue;

        m_commands.append("-- new column " + newColumns[i].newName);
        m_commands.append(dbEngine->getAlterTableForNewColumn(t2->getName(), ct2, newColumns[i].afterColumn));
    }

    if(renamedColumns.size()) m_commands.append("\n");
    for(int i=0; i< renamedColumns.size() ; i++)
    {
        Column* ct2 = t2->getColumn(renamedColumns[i].newName);
        if(!ct2) ct2 = t2->getColumnFromParents(renamedColumns[i].newName);
        if(!ct2) continue;

        m_commands.append("-- rename column " + renamedColumns[i].oldName + " to " + renamedColumns[i].newName);
        m_commands.append(dbEngine->getAlterTableForColumnRename(t2->getName(), ct2, renamedColumns[i].oldName));

    }

    if(columnsThatHaveChangedOrder.size()) m_commands.append("\n");
    for(int i=0; i<columnsThatHaveChangedOrder.size(); i++)
    {
        Column* ct2 = t2->getColumn(columnsThatHaveChangedOrder[i].name);
        if(!ct2) ct2 = t2->getColumnFromParents(columnsThatHaveChangedOrder[i].name);
        if(!ct2) continue;

        m_commands.append("-- column " + columnsThatHaveChangedOrder[i].name + " was relocated " +
                          (columnsThatHaveChangedOrder[i].afterColumn.isEmpty()? " to the first location "
                                                                              : "after " + columnsThatHaveChangedOrder[i].afterColumn));
        m_commands.append(dbEngine->getAlterTableForChangeColumnOrder(t2->getName(), ct2, columnsThatHaveChangedOrder[i].afterColumn));
    }

}
