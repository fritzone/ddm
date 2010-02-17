#ifndef _TABLE_H_
#define _TABLE_H_

#include <QVector>

class Column;

/**
 * The table class holds a database table defined by the user.
 */
class Table
{
public:
    Table();
    void addColumn(Column* column);
    void moveColumnDown(int c);
    void moveColumnUp(int c);
    const Column* getColumn(int c);
private:

    QVector<Column*> m_columns;
};

#endif // TABLE_H
