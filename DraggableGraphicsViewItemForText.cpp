#include "DraggableGraphicsItemForText.h"
#include "ERGraphicsScene.h"
#include "EnterNoteTextDialog.h"
#include "Diagram.h"
#include "DiagramForm.h"

#include <QtGui>
#include <QGraphicsScene>

void DraggableGraphicsViewItemForText::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    qDebug() << "note mousepress : X=" << event->pos().x() << " Y=" << event->pos().y();
    if (event->button() != Qt::LeftButton)
    {
        if(event->button() == Qt::RightButton)
        {
            QMenu popup;
            QAction * action_Remove = new QAction(this->scene());
            action_Remove->setText("Remove from diagram");
            QIcon remove(":/images/actions/images/small/remove.png");
            action_Remove->setIcon(remove);

            QAction * action_Change = new QAction(this->scene());
            action_Change->setText("Change text");


            QObject::connect(action_Remove, SIGNAL(activated()), dynamic_cast<ERGraphicsScene*>(this->scene())->getDiagram()->getDiagramForm(), SLOT(removeNoteFromDiagram()));
            QObject::connect(action_Change, SIGNAL(activated()), dynamic_cast<ERGraphicsScene*>(this->scene())->getDiagram()->getDiagramForm(), SLOT(onEditNote()));
            dynamic_cast<ERGraphicsScene*>(this->scene())->getDiagram()->getDiagramForm()->setCurrentWorkNoteOnDiagram(m_item->toHtml());

            popup.addAction(action_Remove);
            popup.addAction(action_Change);

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
}

void DraggableGraphicsViewItemForText::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    qDebug() << "note mouserelease : X=" << event->pos().x() << " Y=" << event->pos().y();
    event->accept();
    dynamic_cast<ERGraphicsScene*>(this->scene())->setDraggedItem(0);
}

/**/

QString DraggableGraphicsViewItemForText::getText() const
{
    return m_txt;
}

void DraggableGraphicsViewItemForText::removeFromScene()
{
    scene()->removeItem(this);
    if(m_item)
    {

        delete m_item;
        m_item = 0;
    }
    if(m_rect)
    {

        delete m_rect;
        m_rect = 0;
    }
}

void DraggableGraphicsViewItemForText::editNote()
{
    EnterNoteTextDialog* enterText = new EnterNoteTextDialog();
    enterText->setText(m_item->toHtml());
    enterText->setFramed(m_rect != 0);
    enterText->setModal(true);
    if(enterText->exec() == QDialog::Accepted)
    {
        m_item->setHtml(enterText->getText());
        if(enterText->isFramed())
        {
            if(m_rect)
            {
                m_rect->setRect(m_item->boundingRect());
            }
            else
            {
                m_rect = new QGraphicsRectItem(m_item->boundingRect());
                m_rect->setZValue(-1);
                m_rect->setBrush(QBrush(Qt::white));
                addToGroup(m_rect);
                m_rect->setX(m_item->x());
                m_rect->setY(m_item->y());
            }
        }
        else
        {
            if(m_rect)
            {
                delete m_rect;
                m_rect = 0;
            }
        }
    }
}
