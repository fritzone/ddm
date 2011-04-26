#include "CellClose.h"
#include "IconFactory.h"
#include "QueryGraphicsScene.h"
#include "CellCommand.h"

CellClose::CellClose(QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(parent, c, owner),
        m_rect(0)
{
}

QGraphicsItemGroup* CellClose::render(int &x, int &y, int &w, int &h)
{
    QRect r(x, y, CELL_SIZE, CELL_SIZE);
    m_rect = new QGraphicsRectItem(r);
    addToGroup(m_rect);
    QGraphicsPixmapItem* typeIcon = 0;
    typeIcon = new QGraphicsPixmapItem(IconFactory::getCloseIcon().pixmap(CELL_SIZE+1,CELL_SIZE+1), this);
    typeIcon->setX(x);
    typeIcon->setY(y);
    placedAtX = x;
    placedAtY = y;
    return this;
}

void CellClose::updateWidth(int newWidth)
{
    QRect br = boundingRect().toRect();
    placedAtX = newWidth - CELL_SIZE;
    int depl = 8;
    if(dynamic_cast<CellCommand*>(m_parent))
    {
        setX(placedAtX);
    }
    else
    {
        setX(placedAtX);
    }
    QRect br2 = boundingRect().toRect();
    QRect newLocation(placedAtX + CELL_SIZE + depl, placedAtY, CELL_SIZE, CELL_SIZE);    // why the + ???
    m_helper->addNewHotCell(this, newLocation);
}

void CellClose::mouseMove(int x, int y)
{
    QPen thick;
    thick.setWidth(2);
    m_rect->setPen(thick);
}

void CellClose::mouseLeft(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    m_rect->setPen(normal);
}

void CellClose::mousePress(int x, int y)
{
    getParent()->onClose();
}
