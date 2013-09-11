#include "qbr_CellQuerySmallOptionsBoxDiamond.h"

CellQuerySmallOptionsBoxDiamond::CellQuerySmallOptionsBoxDiamond(QSet<OptionsType> types,
                                                           QueryGraphicsHelper *c,
                                                           int level,
                                                           QueryGraphicsItem *parent,
                                                           QueryComponent *owner):
    CellQuerySmallOptionsBox(types, c, level, parent, owner),
    m_diamond(0)
{
}

QGraphicsItemGroup* CellQuerySmallOptionsBoxDiamond::render(int& x, int& y, int& /*w*/, int &/*h*/)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    QPointF p1(x, y+5); QPointF p2(x+5, y + 10); QPointF p3(x+10, y+5); QPointF p4(x+5, y);
    QVector <QPointF> v; v << p1 << p2 << p3 << p4;
    QPolygonF po(v);
    m_diamond = new QGraphicsPolygonItem(po, grp);
    m_helper->addNewHotCell(this, (new QGraphicsRectItem(x, y, 10, 10))->rect().toRect());

    new QGraphicsLineItem(x, y + 5, x + 10, y+5, grp);
    new QGraphicsLineItem(x + 5, y, x + 5, y+10, grp);

    return grp;
}

void CellQuerySmallOptionsBoxDiamond::highlight(const QPen &pen)
{
    m_diamond->setPen(pen);
}
