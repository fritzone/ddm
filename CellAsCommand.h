#ifndef CELLASCOMMAND_H
#define CELLASCOMMAND_H

#include "CellCommand.h"

class CellAsCommand : public CellCommand
{
public:
    CellAsCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return "AS";
    }
    virtual bool hasClose()
    {
        return false;
    }
    virtual void onClose();
    QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
};

#endif // CELLASCOMMAND_H
