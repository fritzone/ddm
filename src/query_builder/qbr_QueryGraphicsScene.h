#ifndef QUERYGRAPHICSSCENE_H
#define QUERYGRAPHICSSCENE_H

#include "qbr_QueryGraphicsHelper.h"

#include <QGraphicsScene>
#include <QMap>

class QueryGraphicsScene : public QGraphicsScene
{
public:
    QueryGraphicsScene(QueryGraphicsHelper* c, QWidget* parent=0);
    virtual ~QueryGraphicsScene();
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
    QueryGraphicsHelper* m_comps;
    QueryGraphicsItem* m_prevHovered;
};

#endif // QUERYGRAPHICSSCENE_H
