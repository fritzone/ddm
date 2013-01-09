#include "qbr_QueryGraphicsScene.h"
#include "qbr_QueryGraphicsItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QDebug>

QueryGraphicsScene::QueryGraphicsScene(QueryGraphicsHelper* c, QWidget* parent):QGraphicsScene((QObject*)parent), m_comps(c), m_prevHovered(0)
{
}

QueryGraphicsScene::~QueryGraphicsScene()
{

}

void QueryGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QueryGraphicsItem* qgi = m_comps->clicked(event->scenePos().x(), event->scenePos().y());
    if(qgi)
    {
        qgi->mousePress(event->screenPos().x(), event->screenPos().y());
    }
}

void QueryGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    QueryGraphicsItem* qgi = m_comps->clicked(event->scenePos().x(), event->scenePos().y());
    if(qgi)
    {
        if(qgi == m_prevHovered) return;
        m_prevHovered = qgi;
        qgi->mouseMove(event->scenePos().x(), event->scenePos().y());
        return;
    }
    m_comps->resetHighlightedItem();
    m_prevHovered = 0;
}
