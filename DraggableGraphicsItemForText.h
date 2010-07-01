#ifndef DRAGGABLEGRAPHICSITEMFORTEXT_H
#define DRAGGABLEGRAPHICSITEMFORTEXT_H

#include "DraggableGraphicsItem.h"

#include <QGraphicsTextItem>

class ForeignKey;

class DraggableGraphicsViewItemForText : public DraggableGraphicsViewItem
{
public:

    friend class ERGraphicsView;

    DraggableGraphicsViewItemForText(const QString& txt) : DraggableGraphicsViewItem(0), m_txt(txt)
    {
        m_item = new QGraphicsTextItem();
        m_item->setHtml(txt);
        QGraphicsRectItem* rect = new QGraphicsRectItem(m_item->boundingRect());
        addToGroup(m_item);
        addToGroup(rect);
    }

protected:

    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:

    QString m_txt;
    QGraphicsTextItem* m_item;

};


#endif // DRAGGABLEGRAPHICSITEMFORTEXT_H
