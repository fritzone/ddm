#include "qbr_CellOrderByCommand.h"
#include "qbr_SelectQueryFromComponent.h"
#include "qbr_CellQuerySmallOptionsBox.h"

CellOrderByCommand::CellOrderByCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellOrderByCommand::getCellBrush()
{
    static QBrush fromBrush = QBrush(QColor(210, 202, 117));
    return fromBrush;
}

CellQuerySmallOptionsBox* CellOrderByCommand::provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
{
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner, CellQuerySmallOptionsBox::SHAPE_RECT);
    return smb;
}

void CellOrderByCommand::onClose()
{
    m_owner->onClose();
}

bool CellOrderByCommand::hasClose()
{
    return true;
}

