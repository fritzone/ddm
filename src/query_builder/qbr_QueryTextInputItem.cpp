#include <QtGui>
#include <QGraphicsScene>
#include "qbr_QueryTextInputItem.h"

QueryTextInputItem::QueryTextInputItem(QGraphicsItem *parent, QGraphicsScene *scene) :
    #if QT_VERSION >= 0x050000
    QGraphicsTextItem(parent)
    #else
    QGraphicsTextItem(parent, scene)
    #endif
{
 // TODO: add tho the scene the object
    scene->addItem(this);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}
QVariant QueryTextInputItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        emit selectedChange(this);
    }
    return value;
}

void QueryTextInputItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void QueryTextInputItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}


void QueryTextInputItem::keyPressEvent(QKeyEvent* /*event*/)
{
    int a = 1;
    if(a==1)
    {
        return;
    }
}
