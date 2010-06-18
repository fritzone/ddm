#ifndef ERGRAPHICSVIEW_H
#define ERGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QtGui>

class ERGraphicsScene;
class Version;

class ERGraphicsView : public QGraphicsView
{
public:

    ERGraphicsView(QWidget* parent, Version* v);

protected:

    virtual void mouseMoveEvent(QMouseEvent *event);

private:

    ERGraphicsScene* scene;

};


#endif // ERGRAPHICSVIEW_H
