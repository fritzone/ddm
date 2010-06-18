#ifndef DRAGGABLEGRAPHICSITEM_H
#define DRAGGABLEGRAPHICSITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsSceneDragDropEvent>

#include <qdebug.h>

class DraggableGraphicsViewItem : public QGraphicsItemGroup
{
public:

    DraggableGraphicsViewItem() : QGraphicsItemGroup()
    {
        setAcceptDrops(true);
    }

    QPixmap getDragImage();



protected:

    virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "Draggable::dragEnter : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->acceptProposedAction();
    }

    virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "Draggable::dragmove : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->acceptProposedAction();
    }

    virtual void dropEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "Draggable::drop : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->acceptProposedAction();
    }

    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
};

#endif // DRAGGABLEGRAPHICSITEM_H
