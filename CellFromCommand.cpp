#include "CellFromCommand.h"
#include "SelectQueryFromComponent.h"
#include "CellQuerySmallOptionsBox.h"

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
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner, CellQuerySmallOptionsBox::SHAPE_RECT);
    return smb;
}

void CellFromCommand::onClose()
{
    m_owner->onClose();
}

bool CellFromCommand::hasClose()
{
    SelectQueryFromComponent* sqf = dynamic_cast<SelectQueryFromComponent*>(m_owner);
    if(m_owner)
    {
        return sqf->allowCloseButton();
    }
    return false;
}
