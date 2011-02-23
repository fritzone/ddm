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
    QSet<CellQuerySmallOptionsBox::OptionsType> t;
    t.insert(CellQuerySmallOptionsBox::OPTIONS_ADD_FROM);
    t.insert(CellQuerySmallOptionsBox::OPTIONS_ADD_WHERE);
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_level, m_parent, m_owner);
    return smb;
}
