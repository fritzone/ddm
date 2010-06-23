#ifndef DRAGGABLEGRAPHICSITEM_H
#define DRAGGABLEGRAPHICSITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsSceneDragDropEvent>

class Table;

class DraggableGraphicsViewItem : public QGraphicsItemGroup
{
public:

    DraggableGraphicsViewItem(Table* tab) : QGraphicsItemGroup(), m_table(tab), lastX(0), lastY(0)
    {
        setAcceptDrops(true);
    }

    QPixmap getDragImage();

    const Table* getTable() const
    {
        return m_table;
    }

    virtual QRectF boundingRect() const;

    void setLastX(qreal x)
    {
        lastX = x;
    }

    void setLastY(qreal y)
    {
        lastY = y;
    }

protected:

    virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->acceptProposedAction();
    }

    virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->acceptProposedAction();
    }

    virtual void dropEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->acceptProposedAction();
    }

    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );


private:

    Table* m_table;
    qreal lastX;
    qreal lastY;
};

#endif // DRAGGABLEGRAPHICSITEM_H
