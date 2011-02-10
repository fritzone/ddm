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

QGraphicsItemGroup* CellCommand::render(int& x, int& y, int& w, int &h)
{
    int lx = x;
    int ly = y;
    int lmw = w; // local max width

    // the text
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();
    QGraphicsTextItem* txt = new QGraphicsTextItem(m_command, grp);
    QFont theFont("Arial", 14, QFont::Bold);
    txt->setFont(theFont);
    txt->setPos(lx + 5, ly + 5);
    int bottom = txt->boundingRect().height() + 2;
    // the background for the command
    w = w<100?100:w;
    lmw = w + lx + 2;
    QRectF rect(lx + 2, ly + 2, w, bottom);

    m_rctCommandFrame = new QGraphicsRectItem(rect, grp);
    m_rctCommandFrame->setBrush(Qt::lightGray);
    QPen tPen;
    tPen.setColor(Qt::black);
    txt->setZValue(1);
    m_rctCommandFrame->setPen(tPen);
    QPen thick;
    thick.setWidth(2);

    y = m_rctCommandFrame->boundingRect().bottom() + 2;
    for(int i = 0; i<m_children.count(); i++)
    {
        x += 15;
        int oldy = y-2;
        int neww = w;
        grp->addToGroup(m_children.at(i)->render(x, y, neww, h));
        int halfway = (oldy + y) / 2 ;
        if(w<neww) {w = neww + 15; lmw = neww + 15;}

        QGraphicsLineItem* l1 = new QGraphicsLineItem(x +5+2-15 , oldy+1, x + 5+2-15, halfway , grp);
        QGraphicsLineItem* l2 = new QGraphicsLineItem(x +5+2-15 , halfway, x + 5+2-15, y, grp);
        QGraphicsLineItem* l3 = new QGraphicsLineItem(x +5+2-15 , halfway, x, halfway, grp);
        //l1->setPen(thick);
        //l2->setPen(thick);
        //l3->setPen(thick);

        x -= 15;
    }

    // the small open box
    QGraphicsLineItem* l1 = new QGraphicsLineItem(x + 5 +2 , y-1, x + 5 + 2, y + 20, grp);
    QGraphicsRectItem* box = new QGraphicsRectItem(x + 2, y+10, 10, 10, grp);
    QGraphicsLineItem* l2 = new QGraphicsLineItem(x +2, y + 15, x + 10 + 2, y + 15, grp);
    //box->setPen(thick);
    //l1->setPen(thick);
    //l2->setPen(thick);
    y = box->boundingRect().bottom() + 5 ;

    // the bigger box rounding the stuff
    w = lx + w;
    QRect rect2(lx, ly, w, y - ly);
    m_colorRect = new QGraphicsRectItem(rect2, grp);
    if(m_type == CellCommand::CELL_FROM)
    {
        m_colorRect->setBrush(QBrush(QColor(180, 212, 217)));
    }
    if(m_type == CellCommand::CELL_SELECT)
    {
        m_colorRect->setBrush(QBrush(QColor(217, 188, 175)));
    }
    if(m_type == CellCommand::CELL_WHERE)
    {
        m_colorRect->setBrush(QBrush(QColor(196, 217, 196)));
    }
    m_colorRect->setZValue(-1);
    w = w<lmw?lmw:w;
    return grp;
}

void CellCommand::updateWidth(int newWidth)
{
    {
    QRect newRect(m_rctCommandFrame->boundingRect().left(), m_rctCommandFrame->boundingRect().top(), newWidth - 4, m_rctCommandFrame->boundingRect().height());
    m_rctCommandFrame->setRect(newRect);
    for(int i = 0; i<m_children.count(); i++)
    {
        m_children.at(i)->updateWidth(newWidth);
    }
    }

    {
    QRect newRect(m_colorRect->boundingRect().left(), m_colorRect->boundingRect().top(), newWidth, m_colorRect->boundingRect().height());
    m_colorRect->setRect(newRect);
    }

}
