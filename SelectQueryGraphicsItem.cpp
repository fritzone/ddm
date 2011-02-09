#include "SelectQueryGraphicsItem.h"
#include "CellCommand.h"
#include "TableGraphicsItem.h"

#include "Workspace.h"
#include "Version.h"

SelectQueryGraphicsItem::SelectQueryGraphicsItem() : m_select(new CellCommand(CellCommand::CELL_SELECT)),
    m_from(new CellCommand(CellCommand::CELL_FROM)), m_where(new CellCommand(CellCommand::CELL_WHERE))
{
    m_from->addChild(new TableGraphicsItem(Workspace::getInstance()->workingVersion()->getTables().at(0)));
}

QGraphicsItemGroup* SelectQueryGraphicsItem::render(int& x, int& y)
{
    addToGroup(m_select->render(x, y));
    addToGroup(m_from->render(x, y));
    addToGroup(m_where->render(x, y));

    return this;
}
