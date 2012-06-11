#ifndef DRAGGABLEGRAPHICSITEMFORTEXT_H
#define DRAGGABLEGRAPHICSITEMFORTEXT_H

#include "dgram_DraggableGraphicsItem.h"

#include <QGraphicsTextItem>
#include <QBrush>

class ForeignKey;

class DraggableGraphicsViewItemForText : public DraggableGraphicsViewItem
{
public:

    friend class ERGraphicsView;

    DraggableGraphicsViewItemForText(const QString& txt, bool framed) : DraggableGraphicsViewItem(0), m_txt(txt), m_framed(framed), m_rect(0)
    {
        m_index = indexCounter ++;
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

    int getIndex() const
    {
        return m_index;
    }

    void removeFromScene();

    void editNote();

    bool isFramed()
    {
        return m_framed;
    }

protected:

    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    //void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:

    QString m_txt;

    bool m_framed;

    // temporary stuff
    QGraphicsTextItem* m_item;
    QGraphicsRectItem* m_rect;
    int m_index;

    static int indexCounter;
};

#endif // DRAGGABLEGRAPHICSITEMFORTEXT_H
