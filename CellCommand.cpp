#include "CellCommand.h"
#include <QFont>
#include <QBrush>
#include <QPen>

QBrush CellCommand::fromBrush = QBrush(QColor(180, 212, 217));
QBrush CellCommand::selectBrush = QBrush(QColor(217, 188, 175));
QBrush CellCommand::whereBrush = QBrush(QColor(196, 217, 196));

CellCommand::CellCommand(CellType type, QueryGraphicsHelper *c, int level, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(parent, c, owner),
        m_type(type), m_close(0), m_level(level)
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
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    // the text
    QGraphicsTextItem* txt = new QGraphicsTextItem(m_command, grp);
    QFont theFont("Arial", 14, QFont::Bold);
    txt->setFont(theFont);
    txt->setPos(lx + 5, ly + 5);
    int bottom = txt->boundingRect().height() + 2;
    // the background for the command
    w = w<txt->boundingRect().width()?txt->boundingRect().width():w;
    lmw = w + lx + 2;
    QRectF rect(lx + 2, ly + 2, w, bottom);

    m_rctCommandFrame = new QGraphicsRectItem(rect, grp);
    m_rctCommandFrame->setBrush(getCellBrush());
    QPen tPen;
    tPen.setColor(Qt::black);
    txt->setZValue(1);
    m_rctCommandFrame->setPen(tPen);
    y = m_rctCommandFrame->boundingRect().bottom() + 2;
    for(int i = 0; i<m_children.count(); i++)
    {
        x += CHILDREN_ALIGNMENT;
        int oldy = y-2;
        int neww = w;
        grp->addToGroup(m_children.at(i)->render(x, y, neww, h));
        int halfway = (oldy + y) / 2 ;
        if(w<neww) {w = neww + CHILDREN_ALIGNMENT; lmw = neww + CHILDREN_ALIGNMENT;}

        QGraphicsLineItem* l1 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , grp);
        QGraphicsLineItem* l2 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway, x + 5+2-CHILDREN_ALIGNMENT, y, grp);
        QGraphicsLineItem* l3 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway, x, halfway, grp);
        x -= CHILDREN_ALIGNMENT;
    }

    // the small open box
    QGraphicsLineItem* l1 = new QGraphicsLineItem(x + 5 +2 , y-1, x + 5 + 2, y + 20, grp);
    QGraphicsRectItem* box = new QGraphicsRectItem(x + 2, y+10, 10, 10, grp);
    QGraphicsLineItem* l2 = new QGraphicsLineItem(x +2, y + 15, x + 10 + 2, y + 15, grp);

    y = box->boundingRect().bottom() + 5 ;

    // the bigger box rounding the stuff
    //w = lx + w;
    QRect rect2(lx, ly, w, y - ly);
    m_colorRect = new QGraphicsRectItem(rect2, grp);
    m_colorRect->setBrush(getCellBrush());
    m_colorRect->setZValue(-1);

    if(m_type == CellCommand::CELL_WHERE || m_type == CellCommand::CELL_SELECT)
    {
        m_close = new CellClose(m_helper, this, m_owner);
        int sx = x, sy = y, sw = w, sh = h;
        int cx = 20;
        int cy = ly + 2;
        m_close->render(cx, cy, w, h);
        grp->addToGroup(m_close);
        m_close->setZValue(2);
        x = sx; y = sy; w = sw; h = sh;
    }

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
    if(m_close)
    {
        QPointF t = mapFromParent(mapFromScene(newWidth - 2, 0));
        m_close->updateWidth(t.x() + m_level * CHILDREN_ALIGNMENT);
    }
    }
}

QBrush CellCommand::getCellBrush()
{
    if(m_type == CellCommand::CELL_FROM)
    {
       return (fromBrush);
    }
    if(m_type == CellCommand::CELL_SELECT)
    {
        return (selectBrush);
    }
    if(m_type == CellCommand::CELL_WHERE)
    {
        return (whereBrush);
    }
}

void CellCommand::onClose()
{

}
