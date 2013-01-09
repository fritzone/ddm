#ifndef CELLCLOSE_H
#define CELLCLOSE_H

#include "qbr_QueryGraphicsItem.h"

class QueryGraphicsHelper;

class CellClose : public QueryGraphicsItem
{

public:
    CellClose(int level, QueryGraphicsHelper*, QueryGraphicsItem*, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose(){}
private:
    int placedAtX;
    int placedAtY;
    QGraphicsRectItem* m_rect;
};

#endif // CELLCLOSE_H
