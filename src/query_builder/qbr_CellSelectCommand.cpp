#include "qbr_CellSelectCommand.h"
#include "qbr_CellQuerySmallOptionsBoxRect.h"

CellSelectCommand::CellSelectCommand(QueryGraphicsHelper* c,
                                     int level,
                                     QueryGraphicsItem* parent,
                                     QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellSelectCommand::getCellBrush()
{
    static QBrush selectBrush = QBrush(QColor(217, 188, 175));
    return selectBrush;
}

CellQuerySmallOptionsBox* CellSelectCommand::provideOptionsBox()
{
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBoxRect(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner);
    return smb;
}

void CellSelectCommand::onClose()
{
    m_owner->onClose();
}
