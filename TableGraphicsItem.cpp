#include "TableGraphicsItem.h"
#include "Table.h"
#include "Cell.h"
#include "CellTable.h"

TableGraphicsItem::TableGraphicsItem(Table* tab):m_table(tab), m_tableCell(0)
{
    m_tableCell = new CellTable(tab->getName());
}

QGraphicsItemGroup* TableGraphicsItem::render(int &x, int &y)
{
    addToGroup(m_tableCell->render(x, y));
    return this;
}
