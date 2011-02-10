#include "TableGraphicsItem.h"
#include "Table.h"
#include "Cell.h"
#include "CellTable.h"

TableGraphicsItem::TableGraphicsItem(Table* tab):m_table(tab), m_tableCell(0)
{
    m_tableCell = new CellTable(tab->getName());
}

QGraphicsItemGroup* TableGraphicsItem::render(int &x, int &y, int& w, int &h)
{
    addToGroup(m_tableCell->render(x, y, w, h));
    return this;
}

void TableGraphicsItem::updateWidth(int newWidth)
{
    m_tableCell->updateWidth(newWidth);
}
