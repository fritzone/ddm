#include "qbr_TableGraphicsItem.h"
#include "core_Table.h"
#include "qbr_CellTable.h"
#include "qbr_CellQuerySmallOptionsBox.h"
#include "qbr_CellAsCommand.h"
#include "qbr_CellWhereCommand.h"
#include "qbr_CellJoinCommand.h"

TableGraphicsItem::TableGraphicsItem(const QString& tabName, int level, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(level, parent, c, owner),
        m_tableCell(0), m_as(0), m_joins(), m_ons()
{
    m_tableCell = new CellTable(tabName, level, c, this, owner);
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

        new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , this);  // top
        if(!m_joins.isEmpty())
        {
            new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, this);   // botton
        }
        new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x + 1, halfway+5, this);                    // to right

        // this will be the small options box before the AS
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

    if(!m_joins.isEmpty())
    {
        for(int i=0; i<m_joins.size(); i++)
        {
            int lmw = lw;
            x += CHILDREN_ALIGNMENT;
            int oldy = y-2;
            int neww = lmw - (m_joins.at(i)->getLevel() + 1)* 20;
            addToGroup(m_joins.at(i)->render(x, y, neww, h));
            int halfway = (oldy + 17);
            if(w<neww-20) {w = neww - 20; lmw = neww - 20;}

            if(i == 0)
            {
                new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , this);  // top
            }
            else
            {
                new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy-17, x + 5+2-CHILDREN_ALIGNMENT, halfway , this);  // top, but longer
            }
            new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x + 1, halfway+5, this);                    // to right

            // this will be the small options box before the JOIN
            QSet<OptionsType> t;
            QSet<OptionsType> more = m_joins.at(i)->getOwner()->provideOptions();
            t.unite(more);

            CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_joins.at(i)->getLevel(), m_parent, m_joins.at(i)->getOwner(), CellQuerySmallOptionsBox::SHAPE_DIAMOND);
            int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
            addToGroup(smb->render(tx, ty, tw, th));
            new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, this);   // botton

            x -= CHILDREN_ALIGNMENT;
            w = w<lmw?lmw:w;

            // now render the ON // TODO: UGLY UGLY CODE
            {
            int lmw = lw;
            int neww = lmw - (m_ons.at(i)->getLevel())* 20;
            x += CHILDREN_ALIGNMENT;
            int oldy = y-2;

            addToGroup(m_ons.at(i)->render(x,y,neww,h));

            int halfway = (oldy + 17);
            if(w<neww-20) {w = neww - 20; lmw = neww - 20;}

            new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy+1, x + 5+2-CHILDREN_ALIGNMENT, halfway , this);  // top
            new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x + 1, halfway+5, this);                    // to right

            if(i<m_ons.count() - 1) new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, this);   // bottom, only if not last

            // this will be the small options box before the ON
            QSet<OptionsType> t;
            t.insert(OPTIONS_NEW_WHERE_EXPR);
            t.insert(OPTIONS_NEW_WHERE_EXPR_AND);
            t.insert(OPTIONS_NEW_WHERE_EXPR_OR);

            CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_ons.at(i)->getLevel(), m_parent, m_ons.at(i)->getOwner(), CellQuerySmallOptionsBox::SHAPE_DIAMOND);
            int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
            addToGroup(smb->render(tx, ty, tw, th));

            }

            // done rendering
            x -= CHILDREN_ALIGNMENT;
            w = w<lmw?lmw:w;

            y+=2;
        }
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
    if(!m_joins.isEmpty())
    {
        for(int i=0; i<m_joins.size(); i++)
        {
            m_joins.at(i)->updateWidth(newWidth);
            m_ons.at(i)->updateWidth(newWidth - 2*CELL_SIZE - 6);
        }
    }
}

void TableGraphicsItem::setAs(CellAsCommand* as)
{
    m_as = as;
}

void TableGraphicsItem::addJoin(CellJoinCommand *join, CellWhereCommand* on)
{
    m_joins.append(join);
    m_ons.append(on);
}
