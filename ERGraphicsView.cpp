#include "ERGraphicsView.h"
#include "ERGraphicsScene.h"
#include "Diagram.h"
#include "FkRelationDescriptor.h"
#include "DiagramForm.h"

#include <qdebug.h>

#include <QObject>

ERGraphicsView::ERGraphicsView(QWidget* parent, Version* v, Diagram* dgram, TableListWidget* tlst) : QGraphicsView(parent), lstTables(tlst), m_diagram(dgram), m_mode(Nothing), textItem(0)
{
    setAcceptDrops(true);
    m_scene = new ERGraphicsScene(this, v, dgram, tlst);
    setScene(m_scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ERGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_scene->getJustDropped())
    {
        QPointF scpos = mapToScene(event->pos().x(), event->pos().y());
        m_scene->finalizeItem(scpos.x(), scpos.y());
        return;
    }

    if(m_scene->getDraggedItem() != 0)
    {
        QPointF scpos = mapToScene(event->pos().x(), event->pos().y());
        m_scene->getDraggedItem()->setPos(scpos.x() - m_scene->getStartDragX(), scpos.y() - m_scene->getStartDragY());
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

        // nothing here

    }
    else
    if(m_mode == Note)
    {
        textItem = new DiagramTextItem("<Type Here>");
        //textItem->setFont(myFont);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setZValue(1.0);
        connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)), m_scene->getDiagram()->getDiagramForm(), SLOT(editorLostFocus(DiagramTextItem*)));

        m_scene->addItem(textItem);
        //textItem->setDefaultTextColor(myTextColor);
        textItem->setPos(mapToScene(mouseEvent->pos()));

        m_diagram->m_form->doneNote();
    }
}

void ERGraphicsView::mouseReleaseEvent(QMouseEvent *mouseEvent)
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
