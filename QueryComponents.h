#ifndef QUERYCOMPONENTS_H
#define QUERYCOMPONENTS_H

#include <QRect>
#include <QMap>

class QueryGraphicsItem;

class QueryComponents
{
public:
    QueryComponents();
    void addNewHotCell(QueryGraphicsItem*, QRect);
    QueryGraphicsItem* clicked(int x, int y);

private:

    QMap<QueryGraphicsItem*, QRect> hotCells;
};

#endif // QUERYCOMPONENTS_H
