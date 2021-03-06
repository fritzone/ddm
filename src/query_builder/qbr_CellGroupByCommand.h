#ifndef CELLGROUPBYCOMMAND_H
#define CELLGROUPBYCOMMAND_H

#include "qbr_CellCommand.h"

class CellGroupByCommand : public CellCommand
{
public:
    CellGroupByCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return "GROUP BY";
    }
    virtual bool hasClose();
    virtual void onClose();
    virtual bool hasTypeChooser() {return false;}
    virtual CellQuerySmallOptionsBox* provideOptionsBox();
};


#endif // CELLGROUPBYCOMMAND_H
