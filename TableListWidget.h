#ifndef TABLELISTWIDGET_H
#define TABLELISTWIDGET_H

#include "IconFactory.h"

#include <QListWidget>

class ERGraphicsView;

class TableListWidget : public QListWidget
{
public:

    TableListWidget(QWidget* parent, ERGraphicsView* associatedGrView) : QListWidget(parent), grv(associatedGrView)
    {
        setDragEnabled(true);
    }

protected:
    virtual void dragEnterEvent(QDragEnterEvent *e)
    {
        e->accept();
    }

    virtual void dragLeaveEvent(QDragLeaveEvent *event)
    {
        event->accept();
    }


    virtual void dragMoveEvent(QDragMoveEvent *event)
    {
        event->accept();
    }

    virtual void dropEvent(QDropEvent *event)
    {
        event->accept();
    }

    virtual void mouseMoveEvent(QMouseEvent *event)
    {
        if (!(event->buttons() & Qt::LeftButton)) return;
        if (currentItem() == NULL) return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // mime stuff
        mimeData->setText(currentItem()->text());
        drag->setMimeData(mimeData);
        drag->setPixmap(IconFactory::getTablesIcon().pixmap(16,16));

        // start drag
        drag->start(Qt::CopyAction | Qt::MoveAction);
    }
private:
    ERGraphicsView* grv;
};


#endif // TABLELISTWIDGET_H
