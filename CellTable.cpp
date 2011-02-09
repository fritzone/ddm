#include "CellTable.h"

CellTable::CellTable(const QString& tabName):m_tabName(tabName)
{
}

QGraphicsItemGroup* CellTable::render(int &x, int &y)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();
    QGraphicsTextItem* txt = new QGraphicsTextItem(m_tabName, grp);
    txt->setPos(x,y);
    y += txt->boundingRect().height();
    return grp;
}
