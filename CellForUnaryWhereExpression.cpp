#include "CellClose.h"
#include "CellForUnaryWhereExpression.h"
#include "CellTypeChooser.h"

CellForUnaryWhereExpression::CellForUnaryWhereExpression (int level, QueryGraphicsHelper *c, QueryGraphicsItem *parent, QueryComponent *owner):
    QueryGraphicsItem(level, parent, c, owner), m_frame(0), m_close(0), m_smallTypeModifier(0), m_bigTypeModifier(0)
{
}

QGraphicsItemGroup* CellForUnaryWhereExpression::render(int &x, int &y, int &w, int &h)
{
    int ly = y;
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    QRectF rect(x, y , 10, 20);

    y += 20;

    m_frame = new QGraphicsRectItem(rect, grp);

    m_close = new CellClose(m_level, m_helper, this, m_owner);
    int sx = x, sy = y, sw = w, sh = h;
    int cx = 20;
    int cy = ly;
    m_close->render(cx, cy, w, h);
    grp->addToGroup(m_close);
    m_close->setZValue(2);

    QSet<CellTypeChooser::CellTypeChooserType> allowedTypes;
    allowedTypes.insert(CellTypeChooser::CELLTYPE_NOTHING);

    m_smallTypeModifier = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_REGULAR, CellTypeChooser::CELLTYPE_NOTHING, allowedTypes, m_helper, this, m_owner);
    m_smallTypeModifier ->render(sx, cy, w, h);
    grp->addToGroup(m_smallTypeModifier );
    x = sx; y = sy; w = sw; h = sh;

    return grp;
}

void CellForUnaryWhereExpression::updateWidth(int newWidth)
{
    QRect t1 = m_frame->rect().toRect();
    t1.setWidth(newWidth);
    m_frame->setRect(t1);

    m_close->updateWidth(newWidth + (m_level)*CELL_SIZE + 3);//m_frame->boundingRect().right() - (m_level+1)*CELL_SIZE);
}
