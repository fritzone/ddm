#ifndef QUERYGRAPHICSSCENE_H
#define QUERYGRAPHICSSCENE_H

#include "QueryComponents.h"

#include <QGraphicsScene>
#include <QMap>

class QueryGraphicsScene : public QGraphicsScene
{
public:
    QueryGraphicsScene(QueryComponents* c, QWidget* parent=0);
    virtual ~QueryGraphicsScene();
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
    QueryComponents* m_comps;
    QueryGraphicsItem* m_prevHovered;
};

#endif // QUERYGRAPHICSSCENE_H
