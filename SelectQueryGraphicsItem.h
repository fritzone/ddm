#ifndef SELECTQUERYGRAPHICSITEM_H
#define SELECTQUERYGRAPHICSITEM_H

#include "QueryGraphicsItem.h"

class Cell;

class SelectQueryGraphicsItem : public QueryGraphicsItem
{
public:
    SelectQueryGraphicsItem();

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    void test();

private:
    Cell* m_select;
    Cell* m_from;
    Cell* m_where;
    QGraphicsRectItem* m_frameRect;
};

#endif // SELECTQUERYGRAPHICSITEM_H
