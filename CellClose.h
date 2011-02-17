#ifndef CELLCLOSE_H
#define CELLCLOSE_H

#include "QueryGraphicsItem.h"
#include "QueryComponents.h"

class CellClose : public QueryGraphicsItem
{

public:
    enum { Type = UserType + 10003 };
    int type() const { return Type; }

    CellClose(QueryComponents*, QueryGraphicsItem*);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){};
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
private:
    int placedAtX;
    int placedAtY;
    QueryComponents* m_comps;
    QGraphicsRectItem* m_rect;
    QueryGraphicsItem* m_parent;
};

#endif // CELLCLOSE_H
