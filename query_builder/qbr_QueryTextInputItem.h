#ifndef QUERYTEXTINPUTITEM_H
#define QUERYTEXTINPUTITEM_H

#include <QGraphicsTextItem>

class QueryTextInputItem : public QGraphicsTextItem
{

    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    QueryTextInputItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    int type() const
        { return Type; }

signals:
    void lostFocus(QueryTextInputItem *item);
    void selectedChange(QGraphicsItem *item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void focusOutEvent(QFocusEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // QUERYTEXTINPUTITEM_H
