#include "qbr_CellWhereCommand.h"
#include "qbr_CellQuerySmallOptionsBox.h"

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

CellQuerySmallOptionsBox* CellWhereCommand::provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
{
    if(m_whereType == SelectQueryWhereComponent::WHERETYPE_ON) return 0;
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner, CellQuerySmallOptionsBox::SHAPE_RECT);
    return smb;
}

void CellWhereCommand::onClose()
{

}


