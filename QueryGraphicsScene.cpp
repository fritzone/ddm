#include "QueryGraphicsScene.h"
#include "QueryGraphicsItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QDebug>

QueryGraphicsScene::QueryGraphicsScene(QueryComponents* c, QWidget* parent):QGraphicsScene((QObject*)parent), m_comps(c), m_prevHovered(0)
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
        qgi->mousePress(event->scenePos().x(), event->scenePos().y());
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
    if(!m_prevHovered) return;
    m_prevHovered->mouseLeft(event->scenePos().x(), event->scenePos().y());
    m_prevHovered = 0;
}
