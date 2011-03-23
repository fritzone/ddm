#ifndef CELLGROUPBYCOMMAND_H
#define CELLGROUPBYCOMMAND_H

#include "CellCommand.h"

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
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
};


#endif // CELLGROUPBYCOMMAND_H
