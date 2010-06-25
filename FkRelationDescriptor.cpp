#include "FkRelationDescriptor.h"
#include "DraggableGraphicsItem.h"
#include "DraggableGraphicsItemForForeignKey.h"

#include <QtGui>

void FkRelationDescriptor::updateContent(bool first)
{
    QPointF tab1_brCenter = m_tab1->mapToScene(m_tab1->boundingRect().center());
    QPointF tab2_brCenter = m_tab2->mapToScene(m_tab2->boundingRect().center());

    if(first)
    {
        QRectF bigBR(tab1_brCenter, tab2_brCenter);
        m_fkitm->setX((bigBR.center().x() - m_fkitm->boundingRect().width() /2));
        m_fkitm->setY(bigBR.center().y() - m_fkitm->boundingRect().height() /2);
    }

    QPointF leftPtInScne = m_fkitm->mapToScene(m_fkitm->getLeftPoint());
    QPointF rightPtInScne = m_fkitm->mapToScene(m_fkitm->getRightPoint());
    QPointF toptPtInScne = m_fkitm->mapToScene(m_fkitm->getTopPoint());
    QPointF bottomPtInScne = m_fkitm->mapToScene(m_fkitm->getBottomPoint());

    if(firstLine)
    {
        delete firstLine;
        firstLine = 0;
    }

    // now determine which point is closest to the tab1_brCenter point and draw the line from there
    QPointF closestPoint = closest(tab1_brCenter, leftPtInScne, rightPtInScne, toptPtInScne, bottomPtInScne);

    firstLine = new QGraphicsLineItem(0, 0, tab1_brCenter.x() - closestPoint.x(), tab1_brCenter.y() - closestPoint.y());
    firstLine->setX(closestPoint.x());
    firstLine->setY(closestPoint.y());
    firstLine->setZValue(-2);

    if(secondLine)
    {
        delete secondLine;
        secondLine = 0;
    }
    QPointF closestPoint2 = closest(tab2_brCenter, leftPtInScne, rightPtInScne, toptPtInScne, bottomPtInScne);

    secondLine = new QGraphicsLineItem(0, 0, tab2_brCenter.x() - closestPoint2.x(), tab2_brCenter.y() - closestPoint2.y());
    secondLine->setX(closestPoint2.x());
    secondLine->setY(closestPoint2.y());
    secondLine->setZValue(-2);
}

QPointF FkRelationDescriptor::closest(QPointF to, QPointF p1, QPointF p2, QPointF p3, QPointF p4)
{
    QLineF l1 (p1, to);
    QLineF l2 (p2, to);
    QLineF l3 (p3, to);
    QLineF l4 (p4, to);

    QPointF result = p1;
    qreal minl = l1.length();
    if(l2.length() < minl) { minl = l2.length(); result = p2; }
    if(l3.length() < minl) { minl = l3.length(); result = p3; }
    if(l4.length() < minl) { minl = l4.length(); result = p4; }

    return result;
}

void FkRelationDescriptor::eliberate()
{
    delete firstLine;
    delete secondLine;
    delete m_fkitm;
}
