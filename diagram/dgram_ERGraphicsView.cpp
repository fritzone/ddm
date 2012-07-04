#include "dgram_ERGraphicsView.h"
#include "dgram_ERGraphicsScene.h"
#include "core_Diagram.h"
#include "FkRelationDescriptor.h"
#include "dgram_DraggableGraphicsItemForText.h"
#include "DiagramForm.h"

#include <qdebug.h>

#include <QObject>

ERGraphicsView::ERGraphicsView(QWidget* parent, Version* v, Diagram* dgram, TableListWidget* tlst) : QGraphicsView(parent), lstTables(tlst), m_diagram(dgram), m_mode(Nothing)
{
    setAcceptDrops(true);
    m_scene = new ERGraphicsScene(this, v, dgram, tlst);
    setScene(m_scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setDragMode(QGraphicsView::ScrollHandDrag);
}

ERGraphicsView::~ERGraphicsView()
{
}

void ERGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_scene->getJustDropped() != 0)
    {
        QPointF scpos = mapToScene(event->pos().x(), event->pos().y());
        m_scene->finalizeItem(scpos.x(), scpos.y());
        return;
    }

    if(m_scene->getDraggedItem() != 0)
    {
        QPointF scpos = mapToScene(event->pos().x(), event->pos().y());
        m_scene->getDraggedItem()->setPos(scpos.x() - m_scene->getStartDragX(), scpos.y() - m_scene->getStartDragY());
        m_scene->getDraggedItem()->pSetX(scpos.x() - m_scene->getStartDragX());
        m_scene->getDraggedItem()->pSetY(scpos.y() - m_scene->getStartDragY());
        m_scene->upadteFkrds();
    }
}

// this "forwards" the call to the draggable ... item under the mouse if there is any.
// otherwise it will start the "work" as per the windows's settings ...
void ERGraphicsView::mousePressEvent(QMouseEvent *mouseEvent)
{
    if(m_mode == Nothing)
    {
        for(int i=0; i<m_scene->m_diagram->m_onStage.size(); i++)
        {
            if(m_scene->m_diagram->m_onStage[i]->isUnderMouse())
            {
                QGraphicsSceneMouseEvent* gsme = new QGraphicsSceneMouseEvent(mouseEvent->type());
                QPointF inScene = mapToScene(mouseEvent->pos());

                gsme->setPos( m_scene->m_diagram->m_onStage[i]->mapFromScene( inScene));
                gsme->setButton(mouseEvent->button());
                m_scene->m_diagram->m_onStage[i]->mousePressEvent(gsme);
                return;
            }
        }

        for(int i=0; i<m_scene->m_diagram->m_notes.size(); i++)
        {
            if(m_scene->m_diagram->m_notes[i]->isUnderMouse())
            {
                QGraphicsSceneMouseEvent* gsme = new QGraphicsSceneMouseEvent(mouseEvent->type());
                QPointF inScene = mapToScene(mouseEvent->pos());

                gsme->setPos( m_scene->m_diagram->m_notes[i]->mapFromScene( inScene));
                gsme->setButton(mouseEvent->button());
                m_scene->m_diagram->m_notes[i]->mousePressEvent(gsme);
                return;
            }
        }

        for(int i=0; i<m_scene->m_diagram->m_fksOnStage.size(); i++)
        {
            if(m_scene->m_diagram->m_fksOnStage[i]->getItem()->isUnderMouse())
            {
                QGraphicsSceneMouseEvent* gsme = new QGraphicsSceneMouseEvent(mouseEvent->type());
                QPointF inScene = mapToScene(mouseEvent->pos());

                gsme->setPos( m_scene->m_diagram->m_fksOnStage[i]->getItem()->mapFromScene( inScene));
                gsme->setButton(mouseEvent->button());
                m_scene->m_diagram->m_fksOnStage[i]->getItem()->mousePressEvent(gsme);
                return;
            }
        }

    }
}

void ERGraphicsView::mouseReleaseEvent(QMouseEvent *)
{
    if(m_mode == Nothing)
    {
        m_scene->setDraggedItem(0);
    }
    else
    if(m_mode == Note)
    {
    }
}
