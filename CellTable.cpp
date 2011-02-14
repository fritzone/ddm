#include "CellTable.h"
#include "CellTypeChooser.h"
#include "CellClose.h"
#include "CellTableChooser.h"

#include <QPen>

CellTable::CellTable(const QString& tabName, QueryComponents* c):m_tabName(tabName), m_comps(c)
{
}

// a box at the beginning to change the type
// table name
// box at the end to remove the item
QGraphicsItemGroup* CellTable::render(int &x, int &y, int& w, int &h)
{
    int lx = x;
    int ly = y;
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    m_chooser = new CellTableChooser(m_tabName, m_comps);
    m_chooser->render(lx, ly ,w, h);
    grp->addToGroup(m_chooser);

    QRect rct(lx, ly, m_chooser->boundingRect().width() + 2 * CELL_SIZE + 10, 32);
    m_frame = new QGraphicsRectItem(rct, grp);

    QSet<CellTypeChooser::CellTypeChooserType> allowedTypes;
    allowedTypes.insert(CellTypeChooser::CELLTYPE_TABLE);

    CellTypeChooser* chooser = new CellTypeChooser(CellTypeChooser::CELLTYPE_TABLE, allowedTypes, m_comps);
    chooser->render(lx, ly, w, h);
    grp->addToGroup(chooser);

    m_closer = new CellClose(m_comps);
    int p = m_chooser->boundingRect().width() + CELL_SIZE + 10;
    m_closer->render(p, ly, w, h);
    grp->addToGroup(m_closer);

    y += m_frame->boundingRect().height() + 2;
    return grp;
}

void CellTable::updateWidth(int newWidth)
{
    QRect newRect(m_frame->boundingRect().left(), m_frame->boundingRect().top(), newWidth - CHILDREN_ALIGNMENT - 2, m_frame->boundingRect().height());
    m_frame->setRect(newRect);
    int x = m_frame->boundingRect().right();
    m_closer->updateWidth(x - 10  - Cell::CELL_SIZE);
    m_chooser->updateWidth(newWidth - CHILDREN_ALIGNMENT - 10  - 2 * Cell::CELL_SIZE);
}

