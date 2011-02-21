#include "CellSelectCommand.h"

CellSelectCommand::CellSelectCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellSelectCommand::getCellBrush()
{
    static QBrush selectBrush = QBrush(QColor(217, 188, 175));
    return selectBrush;
}
