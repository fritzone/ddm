#ifndef CELLCOMMAND_H
#define CELLCOMMAND_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryGraphicsHelper.h"
#include "qbr_CellClose.h"

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

    virtual void onClose() = 0;
    virtual QBrush getCellBrush() = 0;
    virtual QString getCommand() = 0;
    virtual bool hasClose() = 0;
    virtual bool hasTypeChooser() = 0;
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) = 0;

    int getLevel() const { return m_level; }

protected:
    QGraphicsRectItem* m_rctCommandFrame;
    QGraphicsRectItem* m_colorRect;
    CellClose* m_close;
    QGraphicsTextItem* m_txt;
};

#endif // CELLCOMMAND_H
