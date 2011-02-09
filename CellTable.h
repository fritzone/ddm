#ifndef CELLTABLE_H
#define CELLTABLE_H

#include "Cell.h"

class CellTable : public Cell
{
public:
    CellTable(const QString& tabName);
    virtual QGraphicsItemGroup* render(int& x, int& y);
private:
    QString m_tabName;
};

#endif // CELLTABLE_H
