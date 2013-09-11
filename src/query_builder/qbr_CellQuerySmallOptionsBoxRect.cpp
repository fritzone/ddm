#include "qbr_CellQuerySmallOptionsBoxRect.h"

CellQuerySmallOptionsBoxRect::CellQuerySmallOptionsBoxRect(QSet<OptionsType> types,
                                                           QueryGraphicsHelper *c,
                                                           int level,
                                                           QueryGraphicsItem *parent,
                                                           QueryComponent *owner):
    CellQuerySmallOptionsBox(types, c, level, parent, owner),
    m_box(0)
{
}

QGraphicsItemGroup* CellQuerySmallOptionsBoxRect::render(int& x, int& y, int& /*w*/, int &/*h*/)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    m_box = new QGraphicsRectItem(x, y, 10, 10, grp);
    m_helper->addNewHotCell(this, m_box->rect().toRect());

    new QGraphicsLineItem(x, y + 5, x + 10, y+5, grp);
    new QGraphicsLineItem(x + 5, y, x + 5, y+10, grp);
    new QGraphicsLineItem(x + 5, y + 5, x + 5, y+16, grp);  // bottom one

    return grp;
}

void CellQuerySmallOptionsBoxRect::highlight(const QPen& pen)
{
    m_box->setPen(pen);
}

