#include "qbr_CellQuerySmallOptionsBoxCircle.h"

CellQuerySmallOptionsBoxCircle::CellQuerySmallOptionsBoxCircle(QSet<OptionsType> types,
                                                           QueryGraphicsHelper *c,
                                                           int level,
                                                           QueryGraphicsItem *parent,
                                                           QueryComponent *owner):
    CellQuerySmallOptionsBox(types, c, level, parent, owner),
    m_ellipse(0)
{
}

QGraphicsItemGroup* CellQuerySmallOptionsBoxCircle::render(int& x, int& y, int& /*w*/, int &/*h*/)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    m_ellipse = new QGraphicsEllipseItem(x, y, 10, 10, grp);
    m_helper->addNewHotCell(this, m_ellipse->rect().toRect());

    // TODO: this is common with the other ones, should go up one level
    new QGraphicsLineItem(x, y + 5, x + 10, y+5, grp);
    new QGraphicsLineItem(x + 5, y, x + 5, y+10, grp);

    return grp;
}

void CellQuerySmallOptionsBoxCircle::highlight(const QPen &pen)
{
    m_ellipse->setPen(pen);
}
