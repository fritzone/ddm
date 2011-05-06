#include "CellForBinaryWhereExpression.h"

CellForBinaryWhereExpression::CellForBinaryWhereExpression (int level, QueryGraphicsHelper *c, QueryGraphicsItem *parent, QueryComponent *owner):
    QueryGraphicsItem(parent, c, owner)
{
}

QGraphicsItemGroup* CellForBinaryWhereExpression::render(int &x, int &y, int &w, int &h)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    QRectF rect(x, y , 10, 10);

    m_frame = new QGraphicsRectItem(rect, grp);

    return grp;
}

void CellForBinaryWhereExpression::updateWidth(int newWidth)
{

}
