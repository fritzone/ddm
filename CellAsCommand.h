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
    virtual void updateWidth(int newWidth);
    QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
private:
    QGraphicsRectItem* m_textInputRect;
};

#endif // CELLASCOMMAND_H
