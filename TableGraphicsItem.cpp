#include "TableGraphicsItem.h"
#include "Table.h"
#include "qbr_CellTable.h"
#include "qbr_CellQuerySmallOptionsBox.h"
#include "qbr_CellAsCommand.h"
#include "qbr_CellWhereCommand.h"
#include "qbr_CellJoinCommand.h"

TableGraphicsItem::TableGraphicsItem(Table* tab, int level, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(level, parent, c, owner),
        m_table(tab), m_tableCell(0), m_as(0), m_join(0)
{
    m_tableCell = new CellTable(tab->getName(), level, c, this, owner);
    m_level = level;
}

QGraphicsItemGroup* TableGraphicsItem::render(int &x, int &y, int& w, int &h)
{
    int lw = w;
    addToGroup(m_tableCell->render(x, y, w, h));
    if(m_as)
    {
        int lmw = lw;
        x += CHILDREN_ALIGNMENT;
        int oldy = y-2;
        int neww = lmw - (m_as->getLevel() + 1)* 20;
        addToGroup(m_as->render(x, y, neww, h));
        int halfway = (oldy + y) / 2 - 5;
        if(w<neww-20) {w = neww - 20; lmw = neww - 20;}

        QGraphicsLineItem* l1 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , this);  // top
        if(m_join)
        {
            QGraphicsLineItem* l2 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, this);   // botton
        }
        QGraphicsLineItem* l3 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x + 1, halfway+5, this);                    // to right

        // this will be the small options box before the items
        QSet<OptionsType> t;
        QSet<OptionsType> more = m_as->getOwner()->provideOptions();
        t.unite(more);

        CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_as->getLevel(), m_parent, m_as->getOwner(), CellQuerySmallOptionsBox::SHAPE_DIAMOND);
        int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
        addToGroup(smb->render(tx, ty, tw, th));

        x -= CHILDREN_ALIGNMENT;
        w = w<lmw?lmw:w;

        y+=2;

    }

    if(m_join)
    {
        int lmw = lw;
        x += CHILDREN_ALIGNMENT;
        int oldy = y-2;
        int neww = lmw - (m_join->getLevel() + 1)* 20;
        addToGroup(m_join->render(x, y, neww, h));
        int halfway = (oldy + 17);
        if(w<neww-20) {w = neww - 20; lmw = neww - 20;}

        QGraphicsLineItem* l1 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , this);  // top
        QGraphicsLineItem* l3 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x + 1, halfway+5, this);                    // to right

        // this will be the small options box before the items
        QSet<OptionsType> t;
        QSet<OptionsType> more = m_join->getOwner()->provideOptions();
        t.unite(more);

        CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_join->getLevel(), m_parent, m_join->getOwner(), CellQuerySmallOptionsBox::SHAPE_DIAMOND);
        int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
        addToGroup(smb->render(tx, ty, tw, th));
        QGraphicsLineItem* l2 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, this);   // botton

        x -= CHILDREN_ALIGNMENT;
        w = w<lmw?lmw:w;

        // now render the ON // TODO: UGLY UGLY CODE
        {
        int lmw = lw;
        int neww = lmw - (m_on->getLevel())* 20;
        x += CHILDREN_ALIGNMENT;
        int oldy = y-2;

        addToGroup(m_on->render(x,y,neww,h));

        int halfway = (oldy + 17);
        if(w<neww-20) {w = neww - 20; lmw = neww - 20;}

        QGraphicsLineItem* l1 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , this);  // top
        QGraphicsLineItem* l3 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x + 1, halfway+5, this);                    // to right

        // this will be the small options box before the items
        QSet<OptionsType> t;
        t.insert(OPTIONS_NEW_WHERE_EXPR);

        CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_on->getLevel(), m_parent, m_on->getOwner(), CellQuerySmallOptionsBox::SHAPE_DIAMOND);
        int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
        addToGroup(smb->render(tx, ty, tw, th));

        }

        // done rendering
        x -= CHILDREN_ALIGNMENT;
        w = w<lmw?lmw:w;

        y+=2;

    }
    return this;
}

void TableGraphicsItem::updateWidth(int newWidth)
{
    m_tableCell->updateWidth(newWidth);
    if(m_as)
    {
        m_as->updateWidth(newWidth);
    }
    if(m_join)
    {
        m_join->updateWidth(newWidth);
        m_on->updateWidth(newWidth - 2*CELL_SIZE - 6);
    }
}

void TableGraphicsItem::setAs(CellAsCommand* as)
{
    m_as = as;
}

void TableGraphicsItem::setJoin(CellJoinCommand *join, CellWhereCommand* on)
{
    m_join = join;
    m_on = on;
}
