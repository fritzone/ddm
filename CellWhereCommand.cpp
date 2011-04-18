#include "CellWhereCommand.h"


CellWhereCommand::CellWhereCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner, SelectQueryWhereComponent::WhereType w) :
        CellCommand(c, level, parent, owner), m_whereType(w)
{
}

QBrush CellWhereCommand::getCellBrush()
{
    static QBrush whereBrush = QBrush(QColor(196, 217, 196));
    static QBrush havingBrush = QBrush(QColor(126, 177, 116));
    static QBrush onBrush = QBrush(QColor(224, 163, 188));
    switch(m_whereType)
    {
    case SelectQueryWhereComponent::WHERETYPE_WHERE:
        return whereBrush;
    case SelectQueryWhereComponent::WHERETYPE_HAVING:
        return havingBrush;
    case SelectQueryWhereComponent::WHERETYPE_ON:
        return onBrush;
    }
}

void CellWhereCommand::onClose()
{

}
