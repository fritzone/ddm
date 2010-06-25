#include "ERGraphicsView.h"

#include "ERGraphicsScene.h"
#include <qdebug.h>

ERGraphicsView::ERGraphicsView(QWidget* parent, Version* v, Diagram* dgram, TableListWidget* tlst) : QGraphicsView(parent), lstTables(tlst), m_diagram(dgram)
{
    setAcceptDrops(true);
    m_scene = new ERGraphicsScene(this, v, dgram, tlst);
    setScene(m_scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ERGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_scene->getJustDropped())
    {
        QPointF scpos = mapToScene(event->pos().x(), event->pos().y());
        m_scene->finalizeItem(scpos.x(), scpos.y());
        return;
    }

    if(m_scene->getDraggedItem() != 0)
    {
        QPointF scpos = mapToScene(event->pos().x(), event->pos().y());
        qDebug() << "moving something around";
        m_scene->getDraggedItem()->setPos(scpos.x() - m_scene->getStartDragX(), scpos.y() - m_scene->getStartDragY());
        m_scene->upadteFkrds();
    }
}
