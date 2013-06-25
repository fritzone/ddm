#include "qbr_CellFromCommand.h"
#include "qbr_SelectQueryFromComponent.h"
#include "qbr_CellQuerySmallOptionsBoxRect.h"

CellFromCommand::CellFromCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellFromCommand::getCellBrush()
{
    static QBrush fromBrush = QBrush(QColor(180, 212, 217));
    return fromBrush;
}

CellQuerySmallOptionsBox* CellFromCommand::provideOptionsBox()
{
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBoxRect(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner);
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
