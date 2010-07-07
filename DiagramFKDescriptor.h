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
