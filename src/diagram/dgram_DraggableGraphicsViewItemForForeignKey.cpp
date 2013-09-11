#include "dgram_DraggableGraphicsItemForForeignKey.h"
#include "dgram_ERGraphicsScene.h"

#include <QtGui>
#include <QGraphicsScene>


void DraggableGraphicsViewItemForForeignKey::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(this);
    dynamic_cast<ERGraphicsScene*>(this->scene())->setStartDragPos(event->pos().x(), event->pos().y());
}

void DraggableGraphicsViewItemForForeignKey::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->accept();
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(0);
}

QUuid DraggableGraphicsViewItemForForeignKey::getClassUid() const
{
    return QUuid(uidDraggableGraphicsViewItemForFK);
}
