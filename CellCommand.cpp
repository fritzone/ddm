#include "CellCommand.h"
#include "CellQuerySmallOptionsBox.h"
#include "CellJoinCommand.h"

#include <QFont>
#include <QBrush>
#include <QPen>

CellCommand::CellCommand(QueryGraphicsHelper *c, int level, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(level, parent, c, owner),
        m_close(0), m_txt(0)
{
    m_level = level;
}

QGraphicsItemGroup* CellCommand::render(int& x, int& y, int& w, int &h)
{
    int lx = x;
    int ly = y;
    int lmw = w; // local max width
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    // the text
    m_txt = new QGraphicsTextItem(getCommand(), grp);
    QFont theFont("Arial", 14, QFont::Bold);
    m_txt->setFont(theFont);
    m_txt->setPos(lx + TEXT_MOVE_X, ly + TEXT_MOVE_Y);
    int bottom = m_txt->boundingRect().height() + 2;
    // the background for the command
    w = w<m_txt->boundingRect().width()?m_txt->boundingRect().width():w;
    lmw = w + 2 + 20;
    if(hasTypeChooser())
    {
        lmw += CELL_SIZE;
        w += CELL_SIZE;
    }
    QRectF rect(lx + 2, ly + 2, w, bottom);

    m_rctCommandFrame = new QGraphicsRectItem(rect, grp);
    m_rctCommandFrame->setBrush(getCellBrush());
    m_txt->setZValue(1);
    y = m_rctCommandFrame->boundingRect().bottom() + 2;
    for(int i = 0; i<m_children.count(); i++)
    {
        x += CHILDREN_ALIGNMENT;
        int oldy = y-2;
        int neww = lmw - (m_level + 1)* 20;
        grp->addToGroup(m_children.at(i)->render(x, y, neww, h));
        int halfway = oldy + 17; //(oldy + y) / 2 - 7;
        if(w<neww-20) {w = neww; lmw = neww;}

        QGraphicsLineItem* l1 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , grp); // top
        if(dynamic_cast<CellJoinCommand*>(this))
        {
            if(i<m_children.count() - 1)
            {
                QGraphicsLineItem* l2 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, grp);   // botton
            }
        }
        else
        {
            QGraphicsLineItem* l2 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, grp);   // botton
        }
        QGraphicsLineItem* l3 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x + 1, halfway+5, grp);                   // to right

        // this will be the small options box before the items
        QSet<OptionsType> t;
        t.insert(OPTIONS_DUPLICATE);

        QSet<OptionsType> more = m_children.at(i)->getOwner()->provideOptions();
        t.unite(more);

        if(dynamic_cast<CellJoinCommand*>(this)) // the children of the joins don't have options
        {
            t.clear();
        }

        CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_level, m_parent, m_children.at(i)->getOwner(), CellQuerySmallOptionsBox::SHAPE_DIAMOND);
        int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
        grp->addToGroup(smb->render(tx, ty, tw, th));

        x -= CHILDREN_ALIGNMENT;
    }

    // the small command box (provided by the child classes)
    CellQuerySmallOptionsBox* smb = provideOptionsBox(m_helper, m_level, m_parent, m_owner);
    if(smb)
    {
        QGraphicsLineItem* l1 = new QGraphicsLineItem(x + 5 +2 , y-1, x + 5 + 2, y + 20, grp);
        QGraphicsRectItem* box = new QGraphicsRectItem(x + 2, y+10, 10, 10, grp);
        QGraphicsLineItem* l2 = new QGraphicsLineItem(x +2, y + 15, x + 10 + 2, y + 15, grp);

        int tx = x + 2; int ty = y + 10; int tw = w; int th = h;
        grp->addToGroup(smb->render(tx, ty, tw, th));
        y = box->boundingRect().bottom() + 5 ;
    }


    // the bigger box rounding the stuff
    QRect rect2(lx, ly, w, y - ly);
    m_colorRect = new QGraphicsRectItem(rect2, grp);
    //m_colorRect->setBrush(getCellBrush());
    QPen t;
    t.setWidth(0);
    t.setColor(Qt::transparent);
    m_colorRect->setPen(t);
    m_colorRect->setZValue(-1);

    // is this having a close button?
    if(hasClose())
    {
        m_close = new CellClose(m_level, m_helper, this, m_owner);
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
