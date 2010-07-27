#ifndef DIAGRAMFKDESCRIPTOR_H
#define DIAGRAMFKDESCRIPTOR_H

#include "DiagramObjectDescriptor.h"

#include <QString>
#include <QPointF>

class DiagramFKDescriptor : public DiagramObjectDescriptor
{

public:

    DiagramFKDescriptor(const QString& txt, const QString& txt_2, int x, int y, const QString& fkn) : DiagramObjectDescriptor(txt, x, y), fkName(fkn), m_txt2(txt_2)
    {}

    DiagramFKDescriptor(const QString &txt, const QString &txt_2, int x, int y, qreal ellx, qreal elly, qreal l1otx, qreal l1oty, qreal l1posx,
                        qreal l1posy, qreal l2otx, qreal l2oty, qreal l2posx, qreal l2posy, qreal rel1posx, qreal rel1posy, qreal rel2posx, qreal rel2posy,
                        qreal arrowp1x, qreal arrowp1y, qreal arrowp2x, qreal arrowp2y, qreal arrowposx, qreal arrowposy, const QString& fkn) :
                    DiagramObjectDescriptor(txt, x, y),
                    m_ellipsePos(ellx,elly), line1OtherPoint(l1otx, l1oty), line1PosPoint(l1posx, l1posy), line2OtherPoint(l2otx, l2oty), line2PosPoint(l2posx, l2posy),
                    rel1TxtPos(rel1posx, rel1posy), rel2TxtPos(rel2posx, rel2posy), arrowP1(arrowp1x, arrowp1y), arrowP2(arrowp2x, arrowp2y), arrowPos(arrowposx, arrowposy), fkName(fkn), m_txt2(txt_2)
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

        fkElement.setAttribute("fkn", fkName);

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
    QString fkName;

private:

    // the second table name
    QString m_txt2;


};

#endif // DIAGRAMFKDESCRIPTOR_H
