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
    Table* parentTable;
};

struct DiagramDeletionAction
{
    Diagram* deletedDiagram;
};

struct ProcedureDeletionAction
{
    Procedure* deletedProcedure;
};

struct FunctionDeletionAction
{
    Function* deletedFunction;
};

struct TriggerDeletionAction
{
    Trigger* deletedTrigger;
};

struct ViewDeletionAction
{
    View* deletedView;
};

struct DataTypeDeletionAction
{
    UserDataType* deletedDataType;
};

#endif // ACTIONDELETETABLE_H
