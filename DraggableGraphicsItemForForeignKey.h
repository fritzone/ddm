#ifndef DRAGGABLEGRAPHICSITEMFORFOREIGNKEY_H
#define DRAGGABLEGRAPHICSITEMFORFOREIGNKEY_H

#include "DraggableGraphicsItem.h"

class ForeignKey;

class DraggableGraphicsViewItemForForeignKey : public DraggableGraphicsViewItem
{
public:

    DraggableGraphicsViewItemForForeignKey(ForeignKey* fk) : DraggableGraphicsViewItem(0), m_fk(fk)
    {}

    void setLeftPoint(QPointF p)
    {
        leftPoint = p;
    }

    void setRightPoint(QPointF p)
    {
        rightPoint = p;
    }

    void setTopPoint(QPointF p)
    {
        topPoint = p;
    }

    void setBottomPoint(QPointF p)
    {
        bottomPoint = p;
    }

    QPointF getLeftPoint()
    {
        return leftPoint;
    }

    QPointF getTopPoint()
    {
        return topPoint;
    }

    QPointF getRightPoint()
    {
        return rightPoint;
    }

    QPointF getBottomPoint()
    {
        return bottomPoint;
    }


    QRectF boundingRect() const
    {
        return QRectF(leftPoint.x(), topPoint.y(), rightPoint.x(), bottomPoint.y());
    }

protected:

    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

private:

    ForeignKey* m_fk;

    QPointF leftPoint;
    QPointF rightPoint;
    QPointF topPoint;
    QPointF bottomPoint;
};

#endif // DRAGGABLEGRAPHICSITEMFORFOREIGNKEY_H
