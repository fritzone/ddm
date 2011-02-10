#include "CellTable.h"
#include "CellTypeChooser.h"

CellTable::CellTable(const QString& tabName):m_tabName(tabName)
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
    QGraphicsTextItem* txt = new QGraphicsTextItem(m_tabName, grp);
    txt->setPos(lx + CellTypeChooser::CELL_SIZE + 5, ly);
    QRect rct(lx, ly, txt->boundingRect().width() + CellTypeChooser::CELL_SIZE + CellTypeChooser::CELL_SIZE + 10, txt->boundingRect().height() + 5);
    QGraphicsRectItem* rect = new QGraphicsRectItem(rct, grp);

    QSet<CellTypeChooser::CellTypeChooserType> allowedTypes;
    allowedTypes.insert(CellTypeChooser::CELLTYPE_TABLE);

    CellTypeChooser* chooser = new CellTypeChooser(CellTypeChooser::CELLTYPE_TABLE, allowedTypes);
    chooser->render(lx, ly, w, h);
    grp->addToGroup(chooser);

    y += rect->boundingRect().height() + 2;
    return grp;
}

void CellTable::updateWidth(int newWidth)
{

}
