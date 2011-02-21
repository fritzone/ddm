#include "CellWhereCommand.h"


CellWhereCommand::CellWhereCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellWhereCommand::getCellBrush()
{

    static QBrush whereBrush = QBrush(QColor(196, 217, 196));

    return whereBrush;
}
