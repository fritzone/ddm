#ifndef SELECTQUERYGRAPHICSITEM_H
#define SELECTQUERYGRAPHICSITEM_H

#include "QueryGraphicsItem.h"
#include "QueryComponents.h"

class Cell;

class SelectQueryGraphicsItem : public QueryGraphicsItem
{
public:
    SelectQueryGraphicsItem(QueryComponents* c);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){};
    virtual void mouseMove(int x, int y){};
    virtual void mouseLeft(int x, int y){};
    void test();

private:
    Cell* m_select;
    Cell* m_from;
    Cell* m_where;
    QGraphicsRectItem* m_frameRect;
    QueryComponents* m_comps;
};

#endif // SELECTQUERYGRAPHICSITEM_H
