#ifndef TABLEGRAPHICSITEM_H
#define TABLEGRAPHICSITEM_H

#include "QueryGraphicsItem.h"
#include "QueryComponents.h"

class Table;
class Cell;

class TableGraphicsItem : public QueryGraphicsItem
{
public:

    TableGraphicsItem(Table*, QueryComponents* c);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){};
    virtual void mouseMove(int x, int y){};
    virtual void mouseLeft(int x, int y){};
private:

    Table* m_table;
    Cell* m_tableCell;
    QueryComponents* m_comps;
};

#endif // TABLEGRAPHICSITEM_H