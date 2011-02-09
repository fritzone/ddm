#include "CellCommand.h"
#include <QFont>
#include <QBrush>
#include <QPen>

CellCommand::CellCommand(CellType type): m_type(type)
{
    switch(m_type)
    {
    case CellCommand::CELL_SELECT:
        m_command = "SELECT";
        break;

    case CellCommand::CELL_FROM:
        m_command = "FROM";
        break;
    case CellCommand::CELL_WHERE:
        m_command = "WHERE";
        break;
    }

}

QGraphicsItemGroup* CellCommand::render(int& x, int& y)
{
    int lx = x;
    int ly = y;

    // the text
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();
    QGraphicsTextItem* txt = new QGraphicsTextItem(m_command, grp);
    QFont theFont("Arial", 14, QFont::Bold);
    txt->setFont(theFont);
    txt->setPos(lx + 5, ly + 5);
    int bottom = txt->boundingRect().height() + 2;
    // the background for the command
    QRectF rect(lx + 2, ly +2, 100, bottom);
    QGraphicsRectItem* rct = new QGraphicsRectItem(rect, grp);
    rct->setBrush(Qt::lightGray);
    QPen tPen;
    tPen.setColor(Qt::black);
    txt->setZValue(1);
    rct->setPen(tPen);
    // the small open box
    QGraphicsLineItem* l1 = new QGraphicsLineItem(x + 5 +2 , rect.bottom(), x + 5 + 2, rect.bottom() + 20, grp);
    QGraphicsRectItem* box = new QGraphicsRectItem(x + 2, rect.bottom()+10, 10, 10, grp);
    QGraphicsLineItem* l2 = new QGraphicsLineItem(x +2, rect.bottom() + 15, x + 10 + 2, rect.bottom() + 15, grp);
    y = box->boundingRect().bottom() + 5 ;
    for(int i = 0; i<m_children.count(); i++)
    {
        x += 15;
        grp->addToGroup(m_children.at(i)->render(x, y));
        x -= 15;
    }
    // the bigger box rounding the stuff
    QRect rect2(lx, ly, 105, y - ly);
    QGraphicsRectItem* rct2 = new QGraphicsRectItem(rect2, grp);
    if(m_type == CellCommand::CELL_FROM)
    {
        rct2->setBrush(QBrush(QColor(180, 212, 217)));
    }
    if(m_type == CellCommand::CELL_SELECT)
    {
        rct2->setBrush(QBrush(QColor(217, 188, 175)));
    }
    if(m_type == CellCommand::CELL_WHERE)
    {
        rct2->setBrush(QBrush(QColor(196, 217, 196)));
    }
    rct2->setZValue(-1);
    return grp;
}
