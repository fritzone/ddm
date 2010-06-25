#ifndef ERGRAPHICSVIEW_H
#define ERGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QtGui>

class ERGraphicsScene;
class Version;
class TableListWidget;
class Diagram;

class ERGraphicsView : public QGraphicsView
{
public:

    ERGraphicsView(QWidget* parent, Version* v, Diagram* dgram, TableListWidget *tlst);

    ERGraphicsScene* scene() const
    {
        return m_scene;
    }

protected:

    virtual void mouseMoveEvent(QMouseEvent *event);

private:

    ERGraphicsScene* m_scene;
    TableListWidget *lstTables;
    Diagram* m_diagram;
};


#endif // ERGRAPHICSVIEW_H
