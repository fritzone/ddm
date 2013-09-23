#ifndef ERGRAPHICSVIEW_H
#define ERGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QtGui>

#include <qdebug.h>

class ERGraphicsScene;
class Version;
class TableListWidget;
class Diagram;


class ERGraphicsView : public QGraphicsView
{
public:
    enum Mode
    {
        Nothing = 0,
        Note = 1
    };

    ERGraphicsView(QWidget* parent, Version* v, Diagram* dgram, TableListWidget *tlst);

    virtual ~ERGraphicsView();

    ERGraphicsScene* scene() const
    {
        return m_scene;
    }
    void setMode(Mode a)
    {
        m_mode = a;
    }

    void disableMouse(bool dis)
    {
        m_mouseDisabled = dis;
    }

protected:

    virtual void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);

private:

    ERGraphicsScene* m_scene;
    TableListWidget *lstTables;
    Diagram* m_diagram;
    Mode m_mode;
    bool m_mouseDisabled;
};


#endif // ERGRAPHICSVIEW_H
