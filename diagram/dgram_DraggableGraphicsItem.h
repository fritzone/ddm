#ifndef DRAGGABLEGRAPHICSITEM_H
#define DRAGGABLEGRAPHICSITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsSceneDragDropEvent>

#include <qdebug.h>

class Table;

class DraggableGraphicsViewItem : public QGraphicsItemGroup
{

    friend class ERGraphicsView;
    
public:

    DraggableGraphicsViewItem(Table* tab) : QGraphicsItemGroup(), m_table(tab), lastX(0), lastY(0)
    {
        setAcceptDrops(true);
    }

    virtual ~DraggableGraphicsViewItem()
    {
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

    void pSetX(int x)
    {
        m_scenePosX = x;
    }

    void pSetY(int y)
    {
        m_scenePosY = y;
    }

    int getpSX()
    {
        return m_scenePosX;
    }

    int getpSY()
    {
        return m_scenePosY;
    }

    void place()
    {
        setX(m_scenePosX);
        setY(m_scenePosY);
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

protected:

    int m_scenePosX;
    int m_scenePosY;
};

#endif // DRAGGABLEGRAPHICSITEM_H
