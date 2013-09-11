#ifndef CELLSELECTCOMMAND_H
#define CELLSELECTCOMMAND_H

#include "qbr_CellCommand.h"

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
    virtual bool hasTypeChooser() {return false;}
    virtual void onClose();
    virtual CellQuerySmallOptionsBox* provideOptionsBox();
};

#endif // CELLSELECTCOMMAND_H
