#include "CellTypeChooser.h"

CellTypeChooser::CellTypeChooser(CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes)
{
}

QGraphicsItemGroup* CellTypeChooser::render(int& x, int& y, int& w, int &h)
{
    QRect r(x, y, CELL_SIZE, CELL_SIZE);
    addToGroup(new QGraphicsRectItem(r));

    return this;
}
