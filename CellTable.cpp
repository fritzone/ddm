#include "CellTable.h"
#include "CellTypeChooser.h"
#include "CellClose.h"
#include "CellTableChooser.h"
#include "QueryGraphicsScene.h"
#include "QueryComponents.h"

#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>

CellTable::CellTable(const QString& tabName, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner):QueryGraphicsItem(parent, c, owner)
        , m_tabName(tabName)
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

    m_chooser = new CellTableChooser(m_tabName, m_helper, this, m_owner);
    m_chooser->render(lx, ly ,w, h);
    grp->addToGroup(m_chooser);

    QRect rct(lx, ly, m_chooser->boundingRect().width() + 2 * CELL_SIZE + 10, 32);
    m_frame = new QGraphicsRectItem(rct, grp);

    QSet<CellTypeChooser::CellTypeChooserType> allowedTypes;
    allowedTypes.insert(CellTypeChooser::CELLTYPE_TABLE);

    CellTypeChooser* chooser = new CellTypeChooser(CellTypeChooser::CELLTYPE_TABLE, allowedTypes, m_helper, this, m_owner);
    chooser->render(lx, ly, w, h);
    grp->addToGroup(chooser);

    m_closer = new CellClose(m_helper, this, m_owner);
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
    m_closer->updateWidth(x - 10  - CELL_SIZE);
    m_chooser->updateWidth(newWidth - CHILDREN_ALIGNMENT - 10  - 2 * CELL_SIZE);
}

void CellTable::updateHeight(int newHeight)
{
    QRect newRect(m_frame->boundingRect().left(), m_frame->boundingRect().top(), m_frame->boundingRect().width(),  m_frame->boundingRect().height() + newHeight);
    m_frame->setRect(newRect);
}

void CellTable::onClose()
{
    m_parent->getOwner()->getParent()->removeChild(m_parent->getOwner()); // first is the table entry, second is the from
    //int h = m_frame->scene()->views().at(0)->horizontalScrollBar()->sliderPosition();
    //int v = m_frame->scene()->views().at(0)->verticalScrollBar()->sliderPosition();

    QPointF centerb = m_frame->scene()->views().at(0)->mapToScene(m_frame->scene()->views().at(0)->viewport()->rect().center());
    int h = centerb.x();
    int v = centerb.y();


    m_helper->triggerReRender(h, v);
}
