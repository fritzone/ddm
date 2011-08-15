#include "CellClose.h"
#include "CellForUnaryWhereExpression.h"
#include "CellTypeChooser.h"
#include "OptionsType.h"
#include "qbr_SingleExpressionQueryComponent.h"

#include <QBrush>

CellForUnaryWhereExpression::CellForUnaryWhereExpression (int level, QueryGraphicsHelper *c, QueryGraphicsItem *parent, QueryComponent *owner):
    QueryGraphicsItem(level, parent, c, owner), m_frame(0), m_close(0), m_smallTypeModifier(0), m_bigTypeModifiers()
{
}

QGraphicsItemGroup* CellForUnaryWhereExpression::render(int &x, int &y, int &w, int &h)
{
    int ly = y;
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    QRectF rect(x, y , 10, CELL_SIZE*2+4);

    m_frame = new QGraphicsRectItem(rect, grp);
    m_frame->setBrush(QBrush(Qt::white));

    m_close = new CellClose(m_level, m_helper, this, m_owner);
    int sx = x, sw = w, sh = h;
    int cx = 20;
    int cy = ly;
    m_close->render(cx, cy, w, h);
    grp->addToGroup(m_close);
    m_close->setZValue(2);
    int extraCounter = 0;   // counts how many extra ct choosers were added ... (such as paranthese, commas, etc)

    SingleExpressionQueryComponent* owner = dynamic_cast<SingleExpressionQueryComponent*>(m_owner);
    if(owner)
    {

        QSet<CellTypeChooserType> allowedTypes;
        QVector<CellTypeChooserType> choosableTypes = owner->getChoosableTypes();

        for(int i=0; i<choosableTypes.size(); i++)
        {
            allowedTypes.insert(choosableTypes.at(i));
        }

        m_smallTypeModifier = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_REGULAR, CELLTYPE_NOTHING, allowedTypes, m_helper, this, m_owner);
        m_smallTypeModifier ->render(sx, cy, w, h);
        grp->addToGroup(m_smallTypeModifier );

        int localXDepl = 0;
        int tx = 0; // this holds where we are putting the next cell type chooser
        int ty = cy+2;
        int i=0;
        QVector<CellTypeChooserType> funcsAndOperators = owner->getFunctionsAndOperators();
        for(i = 0; i<funcsAndOperators.size(); i++)
        {
            QSet<CellTypeChooserType> allowedTypesforBigOne;
            allowedTypesforBigOne.insert(CELLTYPE_REMOVE_THIS);

            CellTypeChooser* bigTypeModifier = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, funcsAndOperators.at(i), allowedTypesforBigOne, m_helper, this, m_owner, i);
            if(owner->hasFunctionAtIndex(i))
            {
                DatabaseBuiltinFunction* func = owner->getFunctionAt(i);
                bigTypeModifier->setFunction(func);
            }
            tx = sx+((CELL_SIZE+1) *2)*(extraCounter + i+1)+2 + localXDepl;
            int localW = w;
            bigTypeModifier->render(tx,ty,localW,h);
            if(localW != w) localXDepl += localW - w + 2;
            if(tx > sw - CELL_SIZE) sw += ((CELL_SIZE+1) *3) +2;
            if(tx + localXDepl > sw - CELL_SIZE) sw += ((CELL_SIZE+1) *3) +2;

            if(owner->hasFunctionAtIndex(i))
            {
                DatabaseBuiltinFunction* func = owner->getFunctionAt(i);

                extraCounter += 1;  // For the first paranthesis
                CellTypeChooser* op_par = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_OPEN_PARANTHESES, allowedTypes, m_helper, this, m_owner);
                tx = sx+((CELL_SIZE+1) *2)*(extraCounter+ i+1)+2 + localXDepl;
                op_par->render(tx,ty,w,h);
                grp->addToGroup(op_par);

                extraCounter += 1;  // For the second paranthesis

                for(int j=0; j<func->getParameterCount(); j++)
                {
                    CellTypeChooser* cursor = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_CURSOR, allowedTypes, m_helper, this, m_owner);
                    tx = sx+((CELL_SIZE+1) *2)*(extraCounter+ i+1)+2 + localXDepl;
                    cursor->render(tx,ty,w,h);
                    grp->addToGroup(cursor);

                    extraCounter ++;
                }

                CellTypeChooser* cl_par = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_CLOSE_PARANTHESES, allowedTypes, m_helper, this, m_owner);
                tx = sx+((CELL_SIZE+1) *2)*(extraCounter+ i+1)+2 + localXDepl;
                cl_par->render(tx,ty,w,h);
                grp->addToGroup(cl_par);
            }

            grp->addToGroup(bigTypeModifier);
            m_bigTypeModifiers.append(bigTypeModifier);
        }

        CellTypeChooser* cursor = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_CURSOR, allowedTypes, m_helper, this, m_owner);
        tx = sx+((CELL_SIZE+1) *2)*(extraCounter+ i+1)+2 + localXDepl;
        cursor->render(tx,ty,w,h);
        grp->addToGroup(cursor);
    }

    sw += 64 + extraCounter * CELL_SIZE;

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
