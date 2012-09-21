#ifndef ACTIONDELETETABLE_H
#define ACTIONDELETETABLE_H

class Table;
class TableInstance;

#include <QList>

struct TableDeletionAction
{
    TableDeletionAction(Table* t) : deletedTable(t), deletedTableInstances() , parentTable(0)
    {}

    Table* deletedTable;
    QList<TableInstance*> deletedTableInstances;
    Table* parentTable;
};

#endif // ACTIONDELETETABLE_H
