#ifndef CELL_H
#define CELL_H

#include <QGraphicsItemGroup>
#include <QList>
#include "QueryGraphicsItem.h"

class Cell : public QueryGraphicsItem
{
public:

    static const int CHILDREN_ALIGNMENT = 15;
    static const int CELL_SIZE = 20;

    Cell();

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h) = 0;
    virtual void updateWidth(int newWidth) = 0;

    void addChild(QueryGraphicsItem*);

protected:

    QList<QueryGraphicsItem*> m_children;
};

#endif // CELL_H
