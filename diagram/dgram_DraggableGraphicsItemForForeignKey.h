#ifndef DRAGGABLEGRAPHICSITEMFORFOREIGNKEY_H
#define DRAGGABLEGRAPHICSITEMFORFOREIGNKEY_H

#include "dgram_DraggableGraphicsItem.h"

class DraggableGraphicsViewItemForForeignKey : public DraggableGraphicsViewItem
{
public:

    friend class ERGraphicsView;

    DraggableGraphicsViewItemForForeignKey() : DraggableGraphicsViewItem(0)
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

    virtual QUuid getClassUid() const;

protected:

    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

private:

    QPointF leftPoint;
    QPointF rightPoint;
    QPointF topPoint;
    QPointF bottomPoint;
};

#endif // DRAGGABLEGRAPHICSITEMFORFOREIGNKEY_H
