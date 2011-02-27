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
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_TABLE);
    t.insert(OPTIONS_NEW_SUBQUERY);
    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_level, m_parent, m_owner);
    return smb;
}
