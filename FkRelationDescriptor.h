#ifndef FKRELATIONDESCRIPTOR_H
#define FKRELATIONDESCRIPTOR_H

#include "DiagramFKDescriptor.h"

#include <QGraphicsLineItem>

class DraggableGraphicsViewItemForForeignKey;
class DraggableGraphicsViewItem;
class ERGraphicsScene;
class ForeignKey;
class Diagram;

class FkRelationDescriptor
{
public:

    /* TODO: something is messed up in the algorithm finding the closest points. Right now it works, but stinks. when you have time fix it.*/
    static const int LEFT = 1;
    static const int RIGHT = 0;
    static const int TOP = 3;
    static const int BOTTOM = 2;

    FkRelationDescriptor(ForeignKey* fk, DraggableGraphicsViewItemForForeignKey* fkitm, DraggableGraphicsViewItem* tab1, DraggableGraphicsViewItem* tab2);

    FkRelationDescriptor() : m_ellipse(0), m_arrowHead(0), rel1Txt(0), rel2Txt(0),
        m_fkitm(0),m_tab1(0), m_tab2(0), m_sentenced(false), firstLine(0), secondLine(0),
        m_descriptor(), m_fk(0), tab1Name(""), tab2Name("")
    {}

    void updateContent(bool first = false);
    
    
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
    
    void eliberate(bool desc_too = true);
    void removeFromScene();
    void recreate(Diagram*);

    void addToScene(ERGraphicsScene*);

    QGraphicsLineItem* getFirstLine() const
    {
        return firstLine;
    }

    QGraphicsLineItem* getSecondLine() const
    {
        return secondLine;
    }

    QGraphicsEllipseItem* m_ellipse;
    QGraphicsPolygonItem* m_arrowHead;

    // TODO: rel1Txt and rel2Txt have inverted roles in the code. would be nice to fix
    QGraphicsTextItem* rel1Txt;
    QGraphicsTextItem* rel2Txt;

    const ForeignKey* getFk() const
    {
        return m_fk;
    }

    DiagramFKDescriptor* descriptor() const
    {
        return m_descriptor;
    }

    void setDescriptor(DiagramFKDescriptor* dfks)
    {
        m_descriptor = dfks;
        tab1Name = dfks->getText();
        tab2Name = dfks->getText2();
    }

private:

    // finds the closes point to "to" from p1..4
    QPointF closest(QPointF, QPointF, QPointF, QPointF, QPointF, int*);

    void freePreviousData();

private:

    DraggableGraphicsViewItemForForeignKey* m_fkitm;
    DraggableGraphicsViewItem* m_tab1;
    DraggableGraphicsViewItem* m_tab2;
    bool m_sentenced ;
    QGraphicsLineItem* firstLine;
    QGraphicsLineItem* secondLine;
    DiagramFKDescriptor* m_descriptor;
    const ForeignKey* m_fk;
    QString tab1Name;
    QString tab2Name;

};

#endif // FKRELATIONDESCRIPTOR_H
