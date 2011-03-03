#include "CellFromCommand.h"

CellFromCommand::CellFromCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellFromCommand::getCellBrush()
{
    static QBrush fromBrush = QBrush(QColor(180, 212, 217));
    return fromBrush;
}

CellQuerySmallOptionsBox* CellFromCommand::provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
{
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner);
    return smb;
}
