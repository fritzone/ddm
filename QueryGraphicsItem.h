#ifndef QUERYGRAPHICSITEM_H
#define QUERYGRAPHICSITEM_H

#include <QGraphicsItemGroup>

/**
 * This is a graphics item representing a query
 */
class QueryGraphicsItem : public QGraphicsItemGroup
{
public:
    QueryGraphicsItem();
    virtual QGraphicsItemGroup* render(int& x, int& y) = 0;
};

#endif // QUERYGRAPHICSITEM_H
