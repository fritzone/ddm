#include "qbr_CellTable.h"
#include "qbr_CellTypeChooser.h"
#include "qbr_CellClose.h"
#include "qbr_CellTableChooser.h"
#include "qbr_QueryGraphicsScene.h"
#include "qbr_QueryComponents.h"
#include "qbr_ColorProvider.h"

#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>

CellTable::CellTable(const QString& tabName, int level, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner):
    QueryGraphicsItem(level, parent, c, owner), m_tabName(tabName)
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

    // the choose contains the table name
    m_chooser = new CellTableChooser(m_tabName, m_level, m_helper, this, m_owner);
    int th = h;
    m_chooser->render(lx, ly ,w, th);
    grp->addToGroup(m_chooser);

    // the blue background for it
    QRect rct(lx, ly, m_chooser->boundingRect().width() + CELL_SIZE + 4, th + 4);
    m_frame = new QGraphicsRectItem(rct, grp);

    m_frame->setBrush( ColorProvider::getTableCellBackgroundBrush() );
    m_frame->setZValue(-4);

    m_closer = new CellClose(m_level, m_helper, this, m_owner);
    int p = m_chooser->boundingRect().width() + CELL_SIZE;
    m_closer->render(p, ly, w, h);
    grp->addToGroup(m_closer);

    y += m_frame->boundingRect().height() + 4;
    return grp;
}

void CellTable::updateWidth(int newWidth)
{
    QRect newRect(m_frame->boundingRect().left(),
                  m_frame->boundingRect().top(),
                  newWidth,
                  m_frame->boundingRect().height());

    m_frame->setRect(newRect);

    m_closer->updateWidth(m_frame->boundingRect().right() - CELL_SIZE);
    m_chooser->updateWidth(newWidth - CHILDREN_ALIGNMENT - CELL_SIZE);
}

void CellTable::updateHeight(int newHeight)
{
    QRect newRect(m_frame->boundingRect().left(),
                  m_frame->boundingRect().top(),
                  m_frame->boundingRect().width(),
                  m_frame->boundingRect().height() + newHeight);

    m_frame->setRect(newRect);
}

void CellTable::onClose()
{
    m_parent->getOwner()->getParent()->removeChild(m_parent->getOwner()); // first is the table entry, second is the from
    m_helper->triggerReRender();
}
