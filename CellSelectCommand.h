#ifndef CELLSELECTCOMMAND_H
#define CELLSELECTCOMMAND_H

#include "CellCommand.h"

class CellSelectCommand : public CellCommand
{
public:
    CellSelectCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return "SELECT";
    }
    virtual bool hasClose()
    {
        if(m_level == 0) return false;
        return true;
    }
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
    {
        return 0;
    }
};

#endif // CELLSELECTCOMMAND_H
