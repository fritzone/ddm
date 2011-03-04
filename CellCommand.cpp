#include "CellCommand.h"
#include "CellQuerySmallOptionsBox.h"

#include <QFont>
#include <QBrush>
#include <QPen>

CellCommand::CellCommand(QueryGraphicsHelper *c, int level, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(parent, c, owner),
        m_close(0), m_level(level)
{
}

QGraphicsItemGroup* CellCommand::render(int& x, int& y, int& w, int &h)
{
    int lx = x;
    int ly = y;
    int lmw = w; // local max width
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    // the text
    QGraphicsTextItem* txt = new QGraphicsTextItem(getCommand(), grp);
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
    txt->setZValue(1);
    y = m_rctCommandFrame->boundingRect().bottom() + 2;
    for(int i = 0; i<m_children.count(); i++)
    {
        x += CHILDREN_ALIGNMENT;
        int oldy = y-2;
        int neww = w;
        grp->addToGroup(m_children.at(i)->render(x, y, neww, h));
        int halfway = (oldy + y) / 2 - 5;
        if(w<neww) {w = neww + CHILDREN_ALIGNMENT; lmw = neww + CHILDREN_ALIGNMENT;}

        QGraphicsLineItem* l1 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , grp); // top
        QGraphicsLineItem* l2 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, grp);   // botton
        QGraphicsLineItem* l3 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x, halfway+5, grp);                   // to right

        QSet<OptionsType> t; t.insert(OPTIONS_DUPLICATE);
        CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_level, m_parent, m_owner);
        int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
        grp->addToGroup(smb->render(tx, ty, tw, th));

        x -= CHILDREN_ALIGNMENT;
    }

    // the small command box (provided by the child classes)
    QGraphicsLineItem* l1 = new QGraphicsLineItem(x + 5 +2 , y-1, x + 5 + 2, y + 20, grp);
    QGraphicsRectItem* box = new QGraphicsRectItem(x + 2, y+10, 10, 10, grp);
    QGraphicsLineItem* l2 = new QGraphicsLineItem(x +2, y + 15, x + 10 + 2, y + 15, grp);
    CellQuerySmallOptionsBox* smb = provideOptionsBox(m_helper, m_level, m_parent, m_owner);
    if(smb)
    {
        int tx = x + 2; int ty = y + 10; int tw = w; int th = h;
        grp->addToGroup(smb->render(tx, ty, tw, th));
    }
    y = box->boundingRect().bottom() + 5 ;

    // the bigger box rounding the stuff
    QRect rect2(lx, ly, w, y - ly);
    m_colorRect = new QGraphicsRectItem(rect2, grp);
    m_colorRect->setBrush(getCellBrush());
    m_colorRect->setZValue(-1);

    // is this having a close button?
    if(hasClose())
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
