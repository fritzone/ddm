#ifndef CELLFORUNARYWHEREEXPRESSION_H
#define CELLFORUNARYWHEREEXPRESSION_H

#include "qbr_QueryGraphicsHelper.h"
#include "qbr_QueryGraphicsItem.h"

class CellClose;
class CellTypeChooser;

class CellForSingleExpression : public QueryGraphicsItem
{
public:
    CellForSingleExpression(int level, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose(){}
private:
    void newElement();
private:
    QGraphicsRectItem* m_frame;
    CellClose* m_close;
    CellTypeChooser* m_smallTypeModifier;
    QVector<CellTypeChooser*> m_bigTypeModifiers;
    int m_saveW;
};

#endif // CELLFORUNARYWHEREEXPRESSION_H
