#include "CellClose.h"
#include "CellForUnaryWhereExpression.h"
#include "CellTypeChooser.h"
#include "OptionsType.h"

#include <QBrush>

CellForUnaryWhereExpression::CellForUnaryWhereExpression (int level, QueryGraphicsHelper *c, QueryGraphicsItem *parent, QueryComponent *owner):
    QueryGraphicsItem(level, parent, c, owner), m_frame(0), m_close(0), m_smallTypeModifier(0), m_bigTypeModifier(0)
{
}

QGraphicsItemGroup* CellForUnaryWhereExpression::render(int &x, int &y, int &w, int &h)
{
    int ly = y;
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    QRectF rect(x, y , 10, CELL_SIZE*2+4);

    m_frame = new QGraphicsRectItem(rect, grp);
    m_frame->setBrush(QBrush(Qt::red));

    m_close = new CellClose(m_level, m_helper, this, m_owner);
    int sx = x, sw = w, sh = h;
    int cx = 20;
    int cy = ly;
    m_close->render(cx, cy, w, h);
    grp->addToGroup(m_close);
    m_close->setZValue(2);

    QSet<CellTypeChooserType> allowedTypes;
    allowedTypes.insert(CELLTYPE_NOTHING);
    allowedTypes.insert(CELLTYPE_NOT);
    allowedTypes.insert(CELLTYPE_NEGATE);

    m_smallTypeModifier = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_REGULAR, CELLTYPE_NOTHING, allowedTypes, m_helper, this, m_owner);
    m_smallTypeModifier ->render(sx, cy, w, h);
    grp->addToGroup(m_smallTypeModifier );

    QSet<CellTypeChooserType> allowedTypesforBigOne;
    allowedTypesforBigOne.insert(CELLTYPE_FUNCTION);
    allowedTypesforBigOne.insert(CELLTYPE_LITERAL);
    allowedTypesforBigOne.insert(CELLTYPE_COLUMN);
    m_bigTypeModifier = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_NOTHING, allowedTypesforBigOne, m_helper, this, m_owner);
    int tx = sx + CELL_SIZE + 5;
    int ty = cy + 2;
    m_bigTypeModifier ->render(tx, ty, w, h);
    grp->addToGroup(m_bigTypeModifier );

    x = sx; w = sw; h = sh;

    y += CELL_SIZE*2+4;

    y+=2;
    h += y - cy;

    return grp;
}

void CellForUnaryWhereExpression::updateWidth(int newWidth)
{
    QRect t1 = m_frame->rect().toRect();
    t1.setWidth(newWidth);
    m_frame->setRect(t1);

    m_close->updateWidth(newWidth + (m_level)*CELL_SIZE + 3);//m_frame->boundingRect().right() - (m_level+1)*CELL_SIZE);
}
