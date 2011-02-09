#ifndef CELL_H
#define CELL_H

#include <QGraphicsItemGroup>
#include <QList>
#include "QueryGraphicsItem.h"

class Cell
{
public:

    Cell();

    virtual QGraphicsItemGroup* render(int& x, int& y) = 0;

    void addChild(QueryGraphicsItem*);

protected:

    QList<QueryGraphicsItem*> m_children;
};

#endif // CELL_H
