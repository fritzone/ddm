#include "CellForBinaryWhereExpression.h"
#include "CellForUnaryWhereExpression.h"
#include "BinaryWhereExpressionQueryComponent.h"
#include "CellTypeChooser.h"

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

        CellTypeChooserType operation = owner->getOperation();

        QSet<CellTypeChooserType> allowedTypesforBigOne;
        allowedTypesforBigOne.insert(CELLTYPE_FUNCTION);
        allowedTypesforBigOne.insert(CELLTYPE_LITERAL);
        allowedTypesforBigOne.insert(CELLTYPE_COLUMN);
        m_operationType = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_NOTHING, allowedTypesforBigOne, m_helper, this, m_owner);

        m_operationType->render(x, y, w, h);
        grp->addToGroup(m_operationType );

        y += CELL_SIZE * 2 + 2;

        m_right = owner->right()->createGraphicsItem(owner->getHelper(), this);
        QGraphicsItemGroup* grpRight = m_right->render(x, y, w, h);

        grp->addToGroup(grpLeft);
        grp->addToGroup(grpRight);

        QRectF rect(x, ly , 10, y - ly - 2);

        m_frame = new QGraphicsRectItem(rect, grp);
    }
    return grp;
}

void CellForBinaryWhereExpression::updateWidth(int newWidth)
{
    QRect t(m_frame->rect().toRect());
    t.setWidth(newWidth);
    m_frame->setRect(t);
    m_left->updateWidth(newWidth);
    m_right->updateWidth(newWidth);
}
