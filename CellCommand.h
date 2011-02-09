#ifndef CELLCOMMAND_H
#define CELLCOMMAND_H

#include "Cell.h"
#include <QString>

class CellCommand : public Cell
{
public:

    enum CellType
    {
        CELL_SELECT = 1,
        CELL_FROM = 2,
        CELL_WHERE = 3
    } ;

    CellCommand(CellType type);
    virtual QGraphicsItemGroup* render(int& x, int& y);

private:
    CellType m_type;
    QString m_command;
};

#endif // CELLCOMMAND_H
