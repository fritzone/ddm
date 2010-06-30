#include "DraggableGraphicsItemForText.h"
#include "ERGraphicsScene.h"

#include <QtGui>
#include <QGraphicsScene>


void DraggableGraphicsViewItemForText::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    qDebug() << "fk mousepress : X=" << event->pos().x() << " Y=" << event->pos().y();
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(this);
    dynamic_cast<ERGraphicsScene*>(this->scene())->setStartDragPos(event->pos().x(), event->pos().y());
}

void DraggableGraphicsViewItemForText::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    qDebug() << "fk mouserelease : X=" << event->pos().x() << " Y=" << event->pos().y();
    event->accept();
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(0);
}


void DraggableGraphicsViewItemForText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

    qDebug () << "Double Click on sg";
    event->accept();
}
