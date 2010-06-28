#ifndef ERGRAPHICSSCENE_H
#define ERGRAPHICSSCENE_H

#include "Version.h"
#include "Table.h"

#include <QGraphicsScene>
#include <QMimeData>

#include "DraggableGraphicsItem.h"
#include "DraggableGraphicsItemForForeignKey.h"
#include "FkRelationDescriptor.h"

class TableListWidget;
class Diagram;

class ERGraphicsScene : public QGraphicsScene
{
public:

    friend class ERGraphicsView;

    static const int WIDTH = 20000;
    static const int HEIGHT = 20000;
    static const int LEFT = -10000;
    static const int TOP = -10000;

    ERGraphicsScene(QWidget* parent, Version* v, Diagram* dgram, TableListWidget *lstTables);

    virtual ~ERGraphicsScene(){}

    void upadteFkrds();

    /**
     * Called on the first mouse move after the drop item ... weird, but works.
     */
    void finalizeItem(int x, int y);

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

    QRectF getCoverageRect();

    Diagram* getDiagram()
    {
        return m_diagram;
    }

    void removeTable(const QString& tabName);

protected:

    /**
     * When the user dropped something from the tree view
     */
    virtual void dropEvent ( QGraphicsSceneDragDropEvent * event );

    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->acceptProposedAction();
    }

    void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->acceptProposedAction();
    }

private:

    DraggableGraphicsViewItem* getItemForTable(const QString& tabName);


private:

    // this represents the item that was just dragged in from the listview of tables
    DraggableGraphicsViewItem* itm;

    // the version in which we are working
    Version* m_version;

    // if we have just dropped an item from the tree view
    bool justDropped;

    // when we start dragging
    int startDragX;
    int startDragY;

    // this is the dragegd item
    DraggableGraphicsViewItem* m_draggedItem;

    TableListWidget *m_lstTables;

    Diagram* m_diagram;
};


#endif // ERGRAPHICSSCENE_H
