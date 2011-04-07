#include "CellSelectCommand.h"
#include "CellQuerySmallOptionsBox.h"

CellSelectCommand::CellSelectCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellSelectCommand::getCellBrush()
{
    static QBrush selectBrush = QBrush(QColor(217, 188, 175));
    return selectBrush;
}

CellQuerySmallOptionsBox* CellSelectCommand::provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
{
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner, CellQuerySmallOptionsBox::SHAPE_RECT);
    return smb;
}

void CellSelectCommand::onClose()
{
    m_owner->onClose();
}
