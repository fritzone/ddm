#ifndef CELLFORBINARYWHEREEXPRESSION_H
#define CELLFORBINARYWHEREEXPRESSION_H

#include "QueryGraphicsHelper.h"
#include "QueryGraphicsItem.h"

class CellForBinaryWhereExpression : public QueryGraphicsItem
{

public:

    CellForBinaryWhereExpression(int level, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose(){}

private:
    QGraphicsRectItem* m_frame;
};

#endif // CELLFORBINARYWHEREEXPRESSION_H
