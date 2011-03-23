#include "TableGraphicsItem.h"
#include "Table.h"
#include "CellTable.h"
#include "CellQuerySmallOptionsBox.h"
#include "CellAsCommand.h"

TableGraphicsItem::TableGraphicsItem(Table* tab, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(parent, c, owner),
        m_table(tab), m_tableCell(0), m_as(0)
{
    m_tableCell = new CellTable(tab->getName(), c, this, owner);
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

        QGraphicsLineItem* l1 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , this); // top
        //QGraphicsLineItem* l2 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, this);   // botton
        QGraphicsLineItem* l3 = new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x, halfway+5, this);                   // to right

        // this will be the small options box before the items
        QSet<OptionsType> t;
        QSet<OptionsType> more = m_as->getOwner()->provideOptions();
        t.unite(more);

        CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_as->getLevel(), m_parent, m_as->getOwner());
        int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
        addToGroup(smb->render(tx, ty, tw, th));

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
        //dynamic_cast<CellTable*>(m_tableCell)->updateHeight(0);
    }
}

void TableGraphicsItem::setAs(CellAsCommand* as)
{
    m_as = as;
}
