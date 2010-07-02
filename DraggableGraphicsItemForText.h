#ifndef DRAGGABLEGRAPHICSITEMFORTEXT_H
#define DRAGGABLEGRAPHICSITEMFORTEXT_H

#include "DraggableGraphicsItem.h"

#include <QGraphicsTextItem>
#include <QBrush>

class ForeignKey;

class DraggableGraphicsViewItemForText : public DraggableGraphicsViewItem
{
public:

    friend class ERGraphicsView;

    DraggableGraphicsViewItemForText(const QString& txt, bool framed) : DraggableGraphicsViewItem(0), m_txt(txt), m_rect(0)
    {
        m_item = new QGraphicsTextItem();
        m_item->setHtml(txt);
        addToGroup(m_item);

        if(framed)
        {
            m_rect = new QGraphicsRectItem(m_item->boundingRect());
            m_rect->setZValue(-1);
            m_rect->setBrush(QBrush(Qt::white));
            addToGroup(m_rect);
        }

    }

    virtual QRectF boundingRect() const
    {
        return m_item->boundingRect();
    }

    QString getText() const;

    void removeFromScene();

    void editNote();


protected:

    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    //void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:

    QString m_txt;
    QGraphicsTextItem* m_item;
    QGraphicsRectItem* m_rect;
};


#endif // DRAGGABLEGRAPHICSITEMFORTEXT_H
