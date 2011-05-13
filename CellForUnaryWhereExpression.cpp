#include "CellClose.h"
#include "CellForUnaryWhereExpression.h"

CellForUnaryWhereExpression::CellForUnaryWhereExpression (int level, QueryGraphicsHelper *c, QueryGraphicsItem *parent, QueryComponent *owner):
    QueryGraphicsItem(parent, c, owner)
{
}

QGraphicsItemGroup* CellForUnaryWhereExpression::render(int &x, int &y, int &w, int &h)
{
    int ly = y;
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    QRectF rect(x, y , 10, 20);

    y+= 20;

    m_frame = new QGraphicsRectItem(rect, grp);

    m_close = new CellClose(m_helper, this, m_owner);
    int sx = x, sy = y, sw = w, sh = h;
    int cx = 20;
    int cy = ly + 2;
    m_close->render(cx, cy, w, h);
    grp->addToGroup(m_close);
    m_close->setZValue(2);
    x = sx; y = sy; w = sw; h = sh;

    return grp;
}

void CellForUnaryWhereExpression::updateWidth(int newWidth)
{
    QRect t1 = m_frame->rect().toRect();
    t1.setWidth(newWidth);
    m_frame->setRect(t1);

    m_close->setX(m_frame->boundingRect().right() - CELL_SIZE);
}
