#ifndef CELLFROMCOMMAND_H
#define CELLFROMCOMMAND_H

#include "CellCommand.h"

class CellQuerySmallOptionsBox;

class CellFromCommand : public CellCommand
{
public:
    CellFromCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return "FROM";
    }
    virtual bool hasClose();
    virtual void onClose();
    virtual bool hasTypeChooser() {return false;}
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
};

#endif // CELLFROMCOMMAND_H
