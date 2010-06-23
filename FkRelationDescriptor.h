#ifndef FKRELATIONDESCRIPTOR_H
#define FKRELATIONDESCRIPTOR_H

#include <QGraphicsLineItem>

class DraggableGraphicsViewItemForForeignKey;
class DraggableGraphicsViewItem;

class FkRelationDescriptor
{
public:
    FkRelationDescriptor(DraggableGraphicsViewItemForForeignKey* fkitm, DraggableGraphicsViewItem* tab1, DraggableGraphicsViewItem* tab2) : firstLine(0), secondLine(0), m_fkitm(fkitm),m_tab1(tab1), m_tab2(tab2)
    {}

    void updateContent(bool first = false);
    QGraphicsLineItem* firstLine;
    QGraphicsLineItem* secondLine;

private:

    // finds the closes point to "to" from p1..4
    QPointF closest(QPointF to, QPointF p1, QPointF p2, QPointF p3, QPointF p4);

private:

    DraggableGraphicsViewItemForForeignKey* m_fkitm;
    DraggableGraphicsViewItem* m_tab1;
    DraggableGraphicsViewItem* m_tab2;

};

#endif // FKRELATIONDESCRIPTOR_H
