#ifndef ERGRAPHICSVIEW_H
#define ERGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QtGui>

class ERGraphicsScene;
class Version;
class TableListWidget;
class Diagram;

class DiagramTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 3 };

    DiagramTextItem(QString txt, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0) : QGraphicsTextItem(txt, parent, scene)
    {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
    }

    int type() const
        { return Type; }

signals:
    void lostFocus(DiagramTextItem *item);
    void selectedChange(QGraphicsItem *item);

protected:

    void focusOutEvent(QFocusEvent *event)
    {
        setTextInteractionFlags(Qt::NoTextInteraction);
        emit lostFocus(this);
        QGraphicsTextItem::focusOutEvent(event);
    }

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
    {
        if (textInteractionFlags() == Qt::NoTextInteraction)
            setTextInteractionFlags(Qt::TextEditorInteraction);
        QGraphicsTextItem::mouseDoubleClickEvent(event);
    }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        if (change == QGraphicsItem::ItemSelectedHasChanged)
            emit selectedChange(this);
        return value;
    }
};


class ERGraphicsView : public QGraphicsView
{
public:
    enum Mode
    {
        Nothing = 0,
        Note = 1
    };

    ERGraphicsView(QWidget* parent, Version* v, Diagram* dgram, TableListWidget *tlst);

    ERGraphicsScene* scene() const
    {
        return m_scene;
    }
    void setMode(Mode a)
    {
        m_mode = a;
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
    DiagramTextItem* textItem ;
};


#endif // ERGRAPHICSVIEW_H
