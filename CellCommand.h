#ifndef CELLCOMMAND_H
#define CELLCOMMAND_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"
#include "CellClose.h"

#include <QBrush>

class CellCommand : public QueryGraphicsItem
{
public:

    enum CellType
    {
        CELL_SELECT = 1,
        CELL_FROM = 2,
        CELL_WHERE = 3
    } ;

    static QBrush fromBrush;
    static QBrush selectBrush;
    static QBrush whereBrush;

    CellCommand(CellType type, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose();
private:
    QBrush getCellBrush();
private:
    CellType m_type;
    QString m_command;
    QGraphicsRectItem* m_rctCommandFrame;
    QGraphicsRectItem* m_colorRect;
    CellClose* m_close;
    int m_level;
};

#endif // CELLCOMMAND_H
