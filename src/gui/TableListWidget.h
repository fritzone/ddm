#ifndef TABLELISTWIDGET_H
#define TABLELISTWIDGET_H

#include "IconFactory.h"

#include <QListWidget>
#include <QDragEnterEvent>
#include <QDrag>

class ERGraphicsView;

class TableListWidget : public QListWidget
{
public:

    TableListWidget(QWidget* parent, ERGraphicsView* associatedGrView) : QListWidget(parent), grv(associatedGrView)
    {
        setDragEnabled(true);
    }

    void removeItem(const QString& str)
    {
        for(int i=0; i<count(); i++)
        {
            if(item(i)->text() == str)
            {
                takeItem(i);
                return;
            }
        }
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
        drag->setPixmap(currentItem()->icon().pixmap(32,32));

        // start drag
        drag->start(Qt::CopyAction | Qt::MoveAction);
    }


private:
    ERGraphicsView* grv;
};


#endif // TABLELISTWIDGET_H
