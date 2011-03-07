#include "CellWhereCommand.h"


CellWhereCommand::CellWhereCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner, bool trueWhere) :
        CellCommand(c, level, parent, owner), m_trueWhere(trueWhere)
{
}

QBrush CellWhereCommand::getCellBrush()
{
    static QBrush whereBrush = QBrush(QColor(196, 217, 196));
    static QBrush havingBrush = QBrush(QColor(126, 177, 116));

    return m_trueWhere?whereBrush:havingBrush;
}

void CellWhereCommand::onClose()
{

}
