#include "qbr_CellForBinaryWhereExpression.h"
#include "qbr_CellForSingleExpression.h"
#include "qbr_BinaryWhereExpressionQueryComponent.h"
#include "qbr_CellTypeChooser.h"

CellForBinaryWhereExpression::CellForBinaryWhereExpression (int level, QueryGraphicsHelper *c, QueryGraphicsItem *parent, QueryComponent *owner):
    QueryGraphicsItem(level, parent, c, owner)
{
}

QGraphicsItemGroup* CellForBinaryWhereExpression::render(int &x, int &y, int &w, int &h)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    BinaryWhereExpressionQueryComponent* owner = dynamic_cast<BinaryWhereExpressionQueryComponent*>(m_owner);
    if(owner)
    {
        int ly = y;
        WhereExpressionQueryComponent* l = owner->left();
        m_left = l->createGraphicsItem(owner->getHelper(), this);
        QGraphicsItemGroup* grpLeft = m_left->render(x, y, w, h);
        grp->addToGroup(grpLeft);

    }
    return grp;
}

void CellForBinaryWhereExpression::updateWidth(int newWidth)
{
    m_left->updateWidth(newWidth);
}
