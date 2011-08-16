#ifndef CELLJOINCOMMAND_H
#define CELLJOINCOMMAND_H

#include "qbr_CellCommand.h"

class CellTypeChooser;

class CellJoinCommand : public CellCommand
{
public:
    CellJoinCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return "JOIN";
    }
    virtual bool hasClose()
    {
        return true;
    }
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose();
    virtual void updateWidth(int newWidth);
    virtual bool hasTypeChooser() {return true;}
    QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
private:
    CellTypeChooser* m_chooser;
};

#endif // CELLJOINCOMMAND_H
