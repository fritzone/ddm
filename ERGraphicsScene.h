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

    static const int WIDTH = 20000;
    static const int HEIGHT = 20000;
    static const int LEFT = -10000;
    static const int TOP = -10000;

    ERGraphicsScene(QWidget* parent, Version* v) : QGraphicsScene(LEFT, TOP, WIDTH, HEIGHT, parent), itm(0), m_version(v), justDropped(false), m_draggedItem(0)
    {
        // to mark the centre of the view
        addLine(0, -10, 0, 10);
        addLine(-10, 0, 10, 0);
    }


    void finalizeItem(int x, int y)
    {
        if(!itm) return;
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

    void setJustDropped(bool jd)
    {
        justDropped = jd;
    }

    void setStartDragPos(int x, int y)
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
        QString tabName = event->mimeData()->text();
        event->acceptProposedAction();
        itm = m_version->getTable(tabName)->getDiagramEntity();
        if(!itm) return;
        justDropped = true;
        addItem(itm);

    }

    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->acceptProposedAction();
    }

    void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
    {
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
