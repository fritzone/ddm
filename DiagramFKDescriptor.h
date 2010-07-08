#ifndef DIAGRAMFKDESCRIPTOR_H
#define DIAGRAMFKDESCRIPTOR_H

#include "DiagramObjectDescriptor.h"

#include <QString>
#include <QPointF>

class DiagramFKDescriptor : public DiagramObjectDescriptor
{

public:

    DiagramFKDescriptor(const QString& txt, const QString& txt_2, int x, int y) : DiagramObjectDescriptor(txt, x, y), m_txt2(txt_2)
    {}

    QString getText2() const
    {
        return m_txt2;
    }

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const
    {
        QDomElement fkElement = doc.createElement("Fk");      // will hold the data in this element
        fkElement.setAttribute("tab1", getText());
        fkElement.setAttribute("tab2", getText2());
        fkElement.setAttribute("x", getX());
        fkElement.setAttribute("y", getY());
        fkElement.setAttribute("ellipseX", m_ellipsePos.x());
        fkElement.setAttribute("ellipseY", m_ellipsePos.y());

        fkElement.setAttribute("line1OthX", line1OtherPoint.x());
        fkElement.setAttribute("line1OthY", line1OtherPoint.y());

        fkElement.setAttribute("line1PosX", line1PosPoint.x());
        fkElement.setAttribute("line1PosY", line1PosPoint.y());

        fkElement.setAttribute("line2OthX", line2OtherPoint.x());
        fkElement.setAttribute("line2OthY", line2OtherPoint.y());

        fkElement.setAttribute("line2PosX", line2PosPoint.x());
        fkElement.setAttribute("line2PosY", line2PosPoint.y());

        fkElement.setAttribute("rel1PosX", rel1TxtPos.x());
        fkElement.setAttribute("rel1PosY", rel1TxtPos.y());

        fkElement.setAttribute("rel2PosX", rel2TxtPos.x());
        fkElement.setAttribute("rel2PosY", rel2TxtPos.y());

        fkElement.setAttribute("arrowP1x", arrowP1.x());
        fkElement.setAttribute("arrowP1y", arrowP1.y());

        fkElement.setAttribute("arrowP2x", arrowP2.x());
        fkElement.setAttribute("arrowP2y", arrowP2.y());

        fkElement.setAttribute("arrowPosx", arrowPos.x());
        fkElement.setAttribute("arrowPosy", arrowPos.y());

        parent.appendChild(fkElement);

    }

    QPointF m_ellipsePos;       // the place of the ellipse

    QPointF line1OtherPoint;    // the first point of the line is (0,0) this is the other point
    QPointF line1PosPoint;      // where the line is placed after created

    QPointF line2OtherPoint;    // the first point of the line is (0,0) this is the other point
    QPointF line2PosPoint;      // where the line is placed after created

    QPointF rel1TxtPos;         // where the first relation text will be placed
    QPointF rel2TxtPos;         // where the second relation text will be placed

    QPointF arrowP1;
    QPointF arrowP2;

    QPointF arrowPos;



private:

    // the second table name
    QString m_txt2;

};

#endif // DIAGRAMFKDESCRIPTOR_H
