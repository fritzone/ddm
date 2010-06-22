#ifndef DRAGGABLEGRAPHICSITEM_H
#define DRAGGABLEGRAPHICSITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsSceneDragDropEvent>

class Table;

class DraggableGraphicsViewItem : public QGraphicsItemGroup
{
public:

    DraggableGraphicsViewItem(Table* tab) : QGraphicsItemGroup(), m_table(tab)
    {
        setAcceptDrops(true);
    }

    QPixmap getDragImage();

    const Table* getTable() const
    {
        return m_table;
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
};

#endif // DRAGGABLEGRAPHICSITEM_H
