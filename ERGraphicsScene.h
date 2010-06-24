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

class ERGraphicsScene : public QGraphicsScene
{
public:

    static const int WIDTH = 20000;
    static const int HEIGHT = 20000;
    static const int LEFT = -10000;
    static const int TOP = -10000;

    ERGraphicsScene(QWidget* parent, Version* v, TableListWidget *lstTables) :  QGraphicsScene(LEFT, TOP, WIDTH, HEIGHT, parent),
                                                                                itm(0), m_version(v), justDropped(false), m_draggedItem(0), m_lstTables(lstTables)
    {
        // to mark the centre of the view
        QPen pen(Qt::lightGray);

        //addLine(0, -5, 0, 5, pen);
        //addLine(-5, 0, 5, 0, pen);

        // should we draw a bunch of pixels ?
    }

    void upadteFkrds()
    {
        for(int i=0; i<m_fksOnStage.size(); i++)
        {
            m_fksOnStage[i]->updateContent();
            addItem(m_fksOnStage[i]->firstLine);
            addItem(m_fksOnStage[i]->secondLine);

        }
    }

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

    // these table elements are already on the stage
    QVector <DraggableGraphicsViewItem*> m_onStage;

    // these foreign key elements are already on the stage
    QVector <FkRelationDescriptor*> m_fksOnStage;

    TableListWidget *m_lstTables;
};


#endif // ERGRAPHICSSCENE_H
