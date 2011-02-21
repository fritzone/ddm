#include "CellQuerySmallOptionsBox.h"

CellQuerySmallOptionsBox::CellQuerySmallOptionsBox(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(parent, c, owner)

{
}

QGraphicsItemGroup* CellQuerySmallOptionsBox::render(int& x, int& y, int& w, int &h)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    //QGraphicsLineItem* l1 = new QGraphicsLineItem(x , y, x, y + 20, grp);
    QGraphicsRectItem* box = new QGraphicsRectItem(x, y, 10, 10, grp);
    //QGraphicsLineItem* l2 = new QGraphicsLineItem(x, y + 15, x , y + 15, grp);

    return grp;
}
