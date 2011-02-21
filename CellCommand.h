#ifndef CELLCOMMAND_H
#define CELLCOMMAND_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"
#include "CellClose.h"

#include <QBrush>

class CellQuerySmallOptionsBox;

class CellCommand : public QueryGraphicsItem
{
public:

    static QBrush fromBrush;
    static QBrush selectBrush;
    static QBrush whereBrush;

    CellCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose();

    virtual QBrush getCellBrush() = 0;
    virtual QString getCommand() = 0;
    virtual bool hasClose() = 0;
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) = 0;

protected:
    QGraphicsRectItem* m_rctCommandFrame;
    QGraphicsRectItem* m_colorRect;
    CellClose* m_close;
    int m_level;
};

#endif // CELLCOMMAND_H
