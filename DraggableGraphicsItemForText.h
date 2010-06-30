#ifndef DRAGGABLEGRAPHICSITEMFORTEXT_H
#define DRAGGABLEGRAPHICSITEMFORTEXT_H

#include "DraggableGraphicsItem.h"

class ForeignKey;

class DraggableGraphicsViewItemForText : public DraggableGraphicsViewItem
{
public:

    friend class ERGraphicsView;

    DraggableGraphicsViewItemForText(const QString& txt) : DraggableGraphicsViewItem(0)
    {}

protected:

    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:

    QString m_txt;

};


#endif // DRAGGABLEGRAPHICSITEMFORTEXT_H
