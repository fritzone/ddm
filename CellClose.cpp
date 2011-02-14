#include "CellClose.h"
#include "IconFactory.h"'
#include "Cell.h"
#include "QueryGraphicsScene.h"

CellClose::CellClose(QueryComponents* c):m_comps(c)
{
}

QGraphicsItemGroup* CellClose::render(int &x, int &y, int &w, int &h)
{
    QRect r(x, y, Cell::CELL_SIZE, Cell::CELL_SIZE);
    addToGroup(new QGraphicsRectItem(r));
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
    placedAtX = newWidth - Cell::CELL_SIZE;
    setX(placedAtX);

    QRect newLocation(placedAtX + Cell::CELL_SIZE + 10, placedAtY, Cell::CELL_SIZE, Cell::CELL_SIZE);    // why the + ???
    m_comps->addNewHotCell(this, newLocation);
}
