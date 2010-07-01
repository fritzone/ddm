#include "DraggableGraphicsItem.h"
#include "ERGraphicsScene.h"
#include "Diagram.h"
#include "DiagramForm.h"

#include <QtGui>
#include <QGraphicsScene>

QPixmap DraggableGraphicsViewItem::getDragImage()
{

    QPixmap result(boundingRect().width(), boundingRect().height());
    result.fill(Qt::blue);

    return result;
}

void DraggableGraphicsViewItem:: mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    qDebug() << "mousepress : X=" << event->pos().x() << " Y=" << event->pos().y();
    if (event->button() != Qt::LeftButton)
    {
        if(event->button() == Qt::RightButton)
        {
            QMenu popup;
            QAction * action_Remove = new QAction(this->scene());
            action_Remove->setText("Remove from graph");
            QIcon remove(":/images/actions/images/small/remove.png");
            action_Remove->setIcon(remove);

            QObject::connect(action_Remove, SIGNAL(activated()), dynamic_cast<ERGraphicsScene*>(this->scene())->getDiagram()->getDiagramForm(), SLOT(removeFromDiagram()));
            dynamic_cast<ERGraphicsScene*>(this->scene())->getDiagram()->getDiagramForm()->setTableToRemoveFromDiagram(m_table->getName());

            popup.addAction(action_Remove);

            popup.exec(QCursor::pos());
            event->accept();
            return;
        }
        else
        {
            event->ignore();
            return;
        }
    }
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(this);
    dynamic_cast<ERGraphicsScene*>(this->scene())->setStartDragPos(event->pos().x(), event->pos().y());
    event->accept();
}

void DraggableGraphicsViewItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    qDebug() << "mouserelease : X=" << event->pos().x() << " Y=" << event->pos().y();
    event->accept();
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(0);
}

QRectF DraggableGraphicsViewItem::boundingRect() const
{
    return QRectF(0,0,lastX,lastY);
}