#include "DraggableGraphicsItem.h"
#include "ERGraphicsScene.h"
#include "Diagram.h"
#include "DiagramForm.h"

#include <QtGui>
#include <QGraphicsScene>

QPixmap DraggableGraphicsViewItem::getDragImage()
{

    QPixmap result((int) boundingRect().width(), (int) boundingRect().height());
    result.fill(Qt::blue);

    return result;
}

void DraggableGraphicsViewItem:: mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    if (event->button() != Qt::LeftButton)
    {
        if(event->button() == Qt::RightButton)
        {
            QMenu popup;
            QAction * action_Remove = new QAction(this->scene());
            action_Remove->setText("Remove from diagram");
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
    dynamic_cast<ERGraphicsScene*>(this->scene())->setStartDragPos((int) event->pos().x(), (int) event->pos().y());
    event->accept();
}

void DraggableGraphicsViewItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->accept();
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(0);
}

QRectF DraggableGraphicsViewItem::boundingRect() const
{
    return QRectF(0,0,lastX,lastY);
}
