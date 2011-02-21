#ifndef CELLWHERECOMMAND_H
#define CELLWHERECOMMAND_H

#include "CellCommand.h"
class CellWhereCommand : public CellCommand
{
public:
    CellWhereCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return "WHERE";
    }
    virtual bool hasClose()
    {
        return true;
    }
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
    {
        return 0;
    }
};

#endif // CELLWHERECOMMAND_H
