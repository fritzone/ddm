#ifndef CELLFORBINARYWHEREEXPRESSION_H
#define CELLFORBINARYWHEREEXPRESSION_H

#include "QueryGraphicsHelper.h"
#include "QueryGraphicsItem.h"

class CellForUnaryWhereExpression;
class CellTypeChooser;

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

    QueryGraphicsItem* m_left;
    QueryGraphicsItem* m_right;

    CellTypeChooser* m_operationType;
};

#endif // CELLFORBINARYWHEREEXPRESSION_H
