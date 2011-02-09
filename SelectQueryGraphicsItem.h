#ifndef SELECTQUERYGRAPHICSITEM_H
#define SELECTQUERYGRAPHICSITEM_H

#include "QueryGraphicsItem.h"

class Cell;

class SelectQueryGraphicsItem : public QueryGraphicsItem
{
public:
    SelectQueryGraphicsItem();

    virtual QGraphicsItemGroup* render(int& x, int& y);

private:
    Cell* m_select;
    Cell* m_from;
    Cell* m_where;
};

#endif // SELECTQUERYGRAPHICSITEM_H
