#ifndef CELLCOMMAND_H
#define CELLCOMMAND_H

#include "Cell.h"
#include "QueryComponents.h"
#include "CellClose.h"

class CellCommand : public Cell
{
public:

     enum CellType
    {
        CELL_SELECT = 1,
        CELL_FROM = 2,
        CELL_WHERE = 3
    } ;

    CellCommand(CellType type, QueryComponents* c, int level);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){};
    virtual void mouseMove(int x, int y){};
    virtual void mouseLeft(int x, int y){};

private:
    CellType m_type;
    QString m_command;
    QGraphicsRectItem* m_rctCommandFrame;
    QGraphicsRectItem* m_colorRect;
    QueryComponents* m_comps;
    CellClose* m_close;
    int m_level;
};

#endif // CELLCOMMAND_H
