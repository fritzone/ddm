#ifndef ERGRAPHICSSCENE_H
#define ERGRAPHICSSCENE_H

#include "Version.h"
#include "Table.h"

#include <QGraphicsScene>
#include <QMimeData>

#include "DraggableGraphicsItem.h"

class ERGraphicsScene : public QGraphicsScene
{
public:
    ERGraphicsScene(QWidget* parent, Version* v) : QGraphicsScene(-10000, -10000, 20000, 20000, parent), itm(0), m_version(v), justDropped(false), m_draggedItem(0)
    {
        // to mark the centre of the view
        addLine(0, -10, 0, 10);
        addLine(-10, 0, 10, 0);
    }


    void finalizeItem(int x, int y)
    {
        itm->setX(x);
        itm->setY(y);

        justDropped = false;
    }

    void setDraggedItem(DraggableGraphicsViewItem* item)
    {
        m_draggedItem = item;
    }

    bool getJustDropped() const
    {
        return justDropped;
    }

    bool setJustDropped(bool jd)
    {
        justDropped = jd;
    }

    bool setStartDragPos(int x, int y)
    {
        startDragX = x;
        startDragY = y;
    }

    int getStartDragX() const
    {
        return startDragX;
    }

    int getStartDragY() const
    {
        return startDragY;
    }

    DraggableGraphicsViewItem* getDraggedItem()
    {
        return m_draggedItem;
    }

protected:
    virtual void dropEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "GraphicsScene::dropEvent : X=" << event->pos().x() << " Y=" << event->pos().y();
        QString tabName = event->mimeData()->text();
        event->acceptProposedAction();

        itm = m_version->getTable(tabName)->prepareDiagramEntity(event->pos().x(), event->pos().y());
        justDropped = true;
        addItem(itm);

    }

    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "GraphicsScene::dragEnter : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->acceptProposedAction();
    }

    void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
    {
        qDebug() << "GraphicsScene::dragMove : X=" << event->pos().x() << " Y=" << event->pos().y();
        event->acceptProposedAction();
    }

private:
    DraggableGraphicsViewItem* itm;
    Version* m_version;
    bool justDropped;
    int startDragX;
    int startDragY;
    DraggableGraphicsViewItem* m_draggedItem;

};


#endif // ERGRAPHICSSCENE_H
