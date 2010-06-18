#include "ERGraphicsView.h"

#include "ERGraphicsScene.h"

ERGraphicsView::ERGraphicsView(QWidget* parent, Version* v) : QGraphicsView(parent)
{
    setAcceptDrops(true);
    scene = new ERGraphicsScene(this, v);
    setScene(scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ERGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(scene->getJustDropped())
    {
        QPointF scpos = mapToScene(event->pos().x(), event->pos().y());
        scene->finalizeItem(scpos.x(), scpos.y());
        return;
    }

    if(scene->getDraggedItem() != 0)
    {
        QPointF scpos = mapToScene(event->pos().x(), event->pos().y());

        scene->getDraggedItem()->setPos(scpos.x() - scene->getStartDragX(), scpos.y() - scene->getStartDragY());
    }
}
