#ifndef ACTIONDELETETABLE_H
#define ACTIONDELETETABLE_H

class Table;
class TableInstance;
class Diagram;
class Procedure;
class Function;
class Trigger;
class View;
class UserDataType;

#include <QList>

struct TableDeletionAction
{
    TableDeletionAction(Table* t) : deletedTable(t), deletedTableInstances() , parentTable(0)
    {}

    Table* deletedTable;
    QList<TableInstance*> deletedTableInstances;
    const Table* parentTable;
};

struct BasicDeletionAction
{
};

template<class T>
struct DeletionAction : public BasicDeletionAction
{
    T* deletedObject;
};

#endif // ACTIONDELETETABLE_H
