#include "TableUpdateGenerator.h"
#include "core_Table.h"
#include "core_Column.h"
#include "UidWarehouse.h"

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

TableUpdateGenerator::TableUpdateGenerator(Table *t1, Table *t2)
{
    QVector<NewColumns> newColumns;     // contains a lis of columns that are new in T2
    QVector<OldNameNewName> renamedColumns;

    // first run. See if T2 has new or renamed columns
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
    }

    // here newColumns and newColumns contain the changes that have happened
    for(int i=0; i< newColumns.size() ; i++)
    {
        qDebug() << "NEW:" << newColumns[i].newName << " after "<< newColumns[i].afterColumn;
    }
    for(int i=0; i< renamedColumns.size() ; i++)
    {
        qDebug() << "REN:" << renamedColumns[i].newName << " from " << renamedColumns[i].oldName;
    }

}
