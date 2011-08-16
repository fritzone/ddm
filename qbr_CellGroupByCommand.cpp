#include "qbr_CellGroupByCommand.h"
#include "qbr_SelectQueryFromComponent.h"
#include "qbr_CellQuerySmallOptionsBox.h"

CellGroupByCommand::CellGroupByCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellGroupByCommand::getCellBrush()
{
    static QBrush fromBrush = QBrush(QColor(170, 202, 117));
    return fromBrush;
}

CellQuerySmallOptionsBox* CellGroupByCommand::provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
{
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner, CellQuerySmallOptionsBox::SHAPE_RECT);
    return smb;
}

void CellGroupByCommand::onClose()
{
    m_owner->onClose();
}

bool CellGroupByCommand::hasClose()
{
    return true;
}

