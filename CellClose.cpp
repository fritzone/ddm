#include "CellClose.h"
#include "IconFactory.h"
#include "Cell.h"
#include "QueryGraphicsScene.h"
#include "CellCommand.h"

CellClose::CellClose(QueryComponents* c, QueryGraphicsItem* itm):m_comps(c), m_rect(0), m_parent(itm)
{
}

QGraphicsItemGroup* CellClose::render(int &x, int &y, int &w, int &h)
{
    QRect r(x, y, Cell::CELL_SIZE, Cell::CELL_SIZE);
    m_rect = new QGraphicsRectItem(r);
    addToGroup(m_rect);
    QGraphicsPixmapItem* typeIcon = 0;
    typeIcon = new QGraphicsPixmapItem(IconFactory::getCloseIcon().pixmap(16,16), this);
    typeIcon->setX(x+2);
    typeIcon->setY(y+2);
    placedAtX = x;
    placedAtY = y;
    return this;
}

void CellClose::updateWidth(int newWidth)
{
    QRect br = boundingRect().toRect();
    placedAtX = newWidth - Cell::CELL_SIZE;
    int depl = 10;
    if(dynamic_cast<CellCommand*>(m_parent))
    {
        depl = 0;
        setX(placedAtX);
    }
    else
    {
        setX(placedAtX);
    }
    QRect br2 = boundingRect().toRect();
    QRect newLocation(placedAtX + Cell::CELL_SIZE + depl, placedAtY, Cell::CELL_SIZE, Cell::CELL_SIZE);    // why the + ???
    m_comps->addNewHotCell(this, newLocation);
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
