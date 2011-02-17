#ifndef SELECTQUERYGRAPHICSITEM_H
#define SELECTQUERYGRAPHICSITEM_H

#include "QueryGraphicsItem.h"
#include "QueryComponents.h"
#include "CellClose.h"

class Cell;

class SelectQueryGraphicsItem : public QueryGraphicsItem
{
public:
    SelectQueryGraphicsItem(QueryComponents* c, int level);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){};
    virtual void mouseMove(int x, int y){};
    virtual void mouseLeft(int x, int y){};
    Cell* getSelect()
    {
        return m_select;
    }
    Cell* getFrom()
    {
        return m_from;
    }

    void test();

private:
    Cell* m_select;
    Cell* m_from;
    Cell* m_where;
    QGraphicsRectItem* m_frameRect;
    QueryComponents* m_comps;
    int m_level;
};

#endif // SELECTQUERYGRAPHICSITEM_H
