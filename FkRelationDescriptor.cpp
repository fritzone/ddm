#include "FkRelationDescriptor.h"
#include "DraggableGraphicsItem.h"
#include "DraggableGraphicsItemForForeignKey.h"
#include "ForeignKey.h"

#include <QtGui>

#include <math.h>

void FkRelationDescriptor::updateContent(bool first)
{
    QPointF tab1_brCenter = m_tab1->mapToScene(m_tab1->boundingRect().center());

    QPointF tab1_leftCenter = m_tab1->mapToScene(0, m_tab1->boundingRect().center().y());
    QPointF tab1_rightCenter = m_tab1->mapToScene(m_tab1->boundingRect().right(), m_tab1->boundingRect().center().y());
    QPointF tab1_topCenter = m_tab1->mapToScene(m_tab1->boundingRect().center().x(), 0);
    QPointF tab1_bottomCenter = m_tab1->mapToScene(m_tab1->boundingRect().center().x(), m_tab1->boundingRect().bottom());

    QPointF tab2_brCenter = m_tab2->mapToScene(m_tab2->boundingRect().center());

    QPointF tab2_leftCenter = m_tab2->mapToScene(0, m_tab2->boundingRect().center().y());
    QPointF tab2_rightCenter = m_tab2->mapToScene(m_tab2->boundingRect().right(), m_tab2->boundingRect().center().y());
    QPointF tab2_topCenter = m_tab2->mapToScene(m_tab2->boundingRect().center().x(), 0);
    QPointF tab2_bottomCenter = m_tab2->mapToScene(m_tab2->boundingRect().center().x(), m_tab2->boundingRect().bottom());

    QPointF tab1SidePoints[] = {tab1_leftCenter, tab1_rightCenter, tab1_topCenter, tab1_bottomCenter};
    QPointF tab2SidePoints[] = {tab2_leftCenter, tab2_rightCenter, tab2_topCenter, tab2_bottomCenter};



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

    // closes point?
    int whichFirst = -1;
    QPointF closestPoint = closest(tab1_brCenter, leftPtInScne, rightPtInScne, toptPtInScne, bottomPtInScne, &whichFirst);

    firstLine = new QGraphicsLineItem(0, 0, tab1SidePoints[whichFirst].x() - closestPoint.x(), tab1SidePoints[whichFirst].y() - closestPoint.y());
    firstLine->setX(closestPoint.x());
    firstLine->setY(closestPoint.y());
    firstLine->setZValue(-2);

    if(secondLine)
    {
        delete secondLine;
        secondLine = 0;
    }

    if(m_ellipse)
    {
        delete m_ellipse;
        m_ellipse = 0;
    }

    if(rel1Txt)
    {
        delete rel1Txt;
        rel1Txt = 0;
    }

    if(rel2Txt)
    {
        delete rel2Txt;
        rel2Txt = 0;
    }

    if(m_arrowHead)
    {
        delete m_arrowHead;
        m_arrowHead = 0;
    }
    
    int whichSecond = -1;
    QPointF closestPoint2 = closest(tab2_brCenter, leftPtInScne, rightPtInScne, toptPtInScne, bottomPtInScne, &whichSecond);

    secondLine = new QGraphicsLineItem(0, 0, tab2SidePoints[whichSecond].x() - closestPoint2.x(), tab2SidePoints[whichSecond].y() - closestPoint2.y());

    m_ellipse = new QGraphicsEllipseItem (tab2SidePoints[whichSecond].x()-5, tab2SidePoints[whichSecond].y()-5, 10, 10);
    m_ellipse->setZValue(-4);
    m_ellipse->setBrush(QBrush(Qt::black));

    rel1Txt = new QGraphicsTextItem(m_fkitm->getFk()->getLocalDescriptiveText());
    QPointF tab2SidePointsForText[4]  = {
        QPoint(tab2_leftCenter.x() - rel1Txt->boundingRect().width(), tab2_leftCenter.y()),
        tab2_rightCenter,
        QPoint(tab2_topCenter.x(), tab2_topCenter.y() - 20),
        tab2_bottomCenter};


    rel1Txt->setPos(tab2SidePointsForText[whichSecond]);


    rel2Txt = new QGraphicsTextItem(m_fkitm->getFk()->getForeignDescriptiveText());

    QPointF tab1SidePointsForText[4]  = {
        QPoint(tab1_leftCenter.x() - rel2Txt->boundingRect().width() - 10, tab1_leftCenter.y()),
        QPoint(tab1_rightCenter.x() + 10, tab1_rightCenter.y()),
        QPoint(tab1_topCenter.x(), tab1_topCenter.y() - 20),
        tab1_bottomCenter};

    QPointF tab1SidePointsForArrow[4]  = {
        QPoint(tab1_leftCenter.x() - 10, tab1_leftCenter.y()),
        tab1_rightCenter,
        QPoint(tab1_topCenter.x(), tab1_topCenter.y() - 10),
        tab1_bottomCenter};


    rel2Txt->setPos(tab1SidePointsForText[whichFirst]);


    QVector<QPointF> polyPoints;    // the arrowhead: will be built on a circle, first point being the side point of the line, second
                                    // being at 30 degrees to the left, other one at 30 degrees to the right, thus forming an equilateral triangle
    double angle = ::acos(firstLine->line().dx() / firstLine->line().length());
    if (firstLine->line().dy() >= 0)
    {
        angle = (M_PI * 2) - angle;
    }
    
    double arrowSize = 10;
    QPointF arrowP1 = firstLine->line().p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                            cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = firstLine->line().p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                            cos(angle + M_PI - M_PI / 3) * arrowSize);

    polyPoints.append(arrowP1);
    polyPoints.append(arrowP2);
    polyPoints.append(QPointF(0,0));

    QPolygonF polyf(polyPoints);
    m_arrowHead = new QGraphicsPolygonItem(polyPoints);
    m_arrowHead->scale(1.2, 1.2);
    m_arrowHead->rotate(180);
    m_arrowHead->setBrush(QBrush(Qt::black));
    m_arrowHead->setZValue(4);

    m_arrowHead->setPos(tab1SidePoints[whichFirst]);

    secondLine->setX(closestPoint2.x());
    secondLine->setY(closestPoint2.y());
    secondLine->setZValue(-2);
}

QPointF FkRelationDescriptor::closest(QPointF to, QPointF left, QPointF right, QPointF top, QPointF bottom, int* which)
{
    QLineF l1 (left, to);
    QLineF l2 (right, to);
    QLineF l3 (top, to);
    QLineF l4 (bottom, to);

    QPointF result = left;
    qreal minl = l1.length();
    *which = LEFT;

    if(l2.length() < minl) { minl = l2.length(); result = right; *which = RIGHT; }
    if(l3.length() < minl) { minl = l3.length(); result = top; *which = TOP; }
    if(l4.length() < minl) { minl = l4.length(); result = bottom; *which = BOTTOM; }

    return result;
}

void FkRelationDescriptor::eliberate()
{
    delete firstLine;
    delete secondLine;
    delete m_fkitm;
}