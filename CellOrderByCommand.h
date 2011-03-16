#ifndef CELLORDERBYCOMMAND_H
#define CELLORDERBYCOMMAND_H

#include "CellCommand.h"

class CellOrderByCommand : public CellCommand
{
public:
    CellOrderByCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return "ORDER BY";
    }
    virtual bool hasClose();
    virtual void onClose();
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
};

#endif // CELLORDERBYCOMMAND_H
