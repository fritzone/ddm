#include "DraggableGraphicsItem.h"
#include "ERGraphicsScene.h"

#include <QtGui>
#include <QGraphicsScene>

QPixmap DraggableGraphicsViewItem::getDragImage()
{

    QPixmap result(boundingRect().width(), boundingRect().height());
    result.fill(Qt::blue);

    return result;
}

void DraggableGraphicsViewItem:: mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    qDebug() << "mousepress : X=" << event->pos().x() << " Y=" << event->pos().y();
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(this);
    dynamic_cast<ERGraphicsScene*>(this->scene())->setStartDragPos(event->pos().x(), event->pos().y());
}

void DraggableGraphicsViewItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    qDebug() << "mouserelease : X=" << event->pos().x() << " Y=" << event->pos().y();
    event->accept();
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(0);
}

QRectF DraggableGraphicsViewItem::boundingRect() const
{
    return QRectF(0,0,lastX,lastY);
}
