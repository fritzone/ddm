#ifndef CORE_COLUMNWITHVALUE_H
#define CORE_COLUMNWITHVALUE_H

class Column;

#include <QString>

struct ColumnWithValue
{
    ColumnWithValue() : column(0), value("") {}
    Column* column;
    QString value;
};

struct FromToColumn
{
    Column* from;
    Column* to;
};

struct ColumnWithValuesAndReference : public ColumnWithValue
{
    FromToColumn* ref;
};

struct OldNameNewName
{
    QString oldName;
    QString newName;
};

#endif // CORE_COLUMNWITHVALUE_H
