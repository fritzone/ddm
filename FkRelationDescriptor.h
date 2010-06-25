#ifndef FKRELATIONDESCRIPTOR_H
#define FKRELATIONDESCRIPTOR_H

#include <QGraphicsLineItem>

class DraggableGraphicsViewItemForForeignKey;
class DraggableGraphicsViewItem;

class FkRelationDescriptor
{
public:
    FkRelationDescriptor(DraggableGraphicsViewItemForForeignKey* fkitm, DraggableGraphicsViewItem* tab1, DraggableGraphicsViewItem* tab2) : 
            firstLine(0), secondLine(0), m_fkitm(fkitm),m_tab1(tab1), m_tab2(tab2), m_sentenced(false)
    {}

    void updateContent(bool first = false);
    
public:    
    
    QGraphicsLineItem* firstLine;
    QGraphicsLineItem*  secondLine;
    
public:
    
    DraggableGraphicsViewItemForForeignKey* getItem()
    {
        return m_fkitm;
    }

    DraggableGraphicsViewItem* getFirst()
    {
        return m_tab1;
    }

    DraggableGraphicsViewItem* getSecond()
    {
        return m_tab2;
    }
    
    void sentence()
    {
        m_sentenced = true;
    }
    
    bool isSentenced() const
    {
        return m_sentenced;
    }
    
    void eliberate();

private:

    // finds the closes point to "to" from p1..4
    QPointF closest(QPointF to, QPointF p1, QPointF p2, QPointF p3, QPointF p4);

private:

    DraggableGraphicsViewItemForForeignKey* m_fkitm;
    DraggableGraphicsViewItem* m_tab1;
    DraggableGraphicsViewItem* m_tab2;
    bool m_sentenced ;
};

#endif // FKRELATIONDESCRIPTOR_H
