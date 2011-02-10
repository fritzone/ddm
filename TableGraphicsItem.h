#ifndef TABLEGRAPHICSITEM_H
#define TABLEGRAPHICSITEM_H

#include "QueryGraphicsItem.h"

class Table;
class Cell;

class TableGraphicsItem : public QueryGraphicsItem
{
public:
    TableGraphicsItem(Table*);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
private:

    Table* m_table;
    Cell* m_tableCell;
};

#endif // TABLEGRAPHICSITEM_H
