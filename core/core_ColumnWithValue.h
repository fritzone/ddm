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

#endif // CORE_COLUMNWITHVALUE_H
