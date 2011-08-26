#include "qbr_CellClose.h"
#include "qbr_CellForSingleExpression.h"
#include "qbr_CellTypeChooser.h"
#include "qbr_OptionTypes.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_DatabaseFunctionInstantiationComponent.h"
#include "qbr_QueryGraphicsItem.h"

#include <QBrush>

CellForSingleExpression::CellForSingleExpression (int level, QueryGraphicsHelper *c, QueryGraphicsItem *parent, QueryComponent *owner):
    QueryGraphicsItem(level, parent, c, owner), m_frame(0), m_close(0), m_smallTypeModifier(0), m_bigTypeModifiers()
{
}

QGraphicsItemGroup* CellForSingleExpression::render(int &x, int &y, int &w, int &h)
{
    int saveW = w;
    int ly = y;
    int sw = w;
    int cx = 20;
    int cy = ly + 2;        // two pixels below the top, this is used only for rendering, it's not modified

    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    if(m_level != -2)
    {
        QRectF rect(x, y , 10, CELL_SIZE*2+4);
        m_frame = new QGraphicsRectItem(rect, grp);
        m_frame->setBrush(QBrush(Qt::white));

        m_close = new CellClose(m_level, m_helper, this, m_owner);
        m_close->render(cx, cy, w, h);
        grp->addToGroup(m_close);
        m_close->setZValue(2);
    }

    if(SingleExpressionQueryComponent* owner = dynamic_cast<SingleExpressionQueryComponent*>(m_owner))
    {
        int tx = x; // this holds where we are putting the next cell type chooser

        QVector<CellTypeChooserType> funcsAndOperators = owner->getFunctionsAndOperators();
        for(int i = 0; i<funcsAndOperators.size(); i++)
        {
            CellTypeChooser* bigTypeModifier = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, funcsAndOperators.at(i), QSet<CellTypeChooserType>(), m_helper, this, m_owner, i);
            bigTypeModifier->setFunction(owner->getFunctionAt(i));

            {
            int localW = 0;
            bigTypeModifier->render(tx, cy, localW, h);
            tx += localW; //bigTypeModifier->boundingRect().width();
            }

            if(const DatabaseBuiltinFunction* func = bigTypeModifier->getFunction())
            {
                CellTypeChooser* op_par = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_OPEN_PARANTHESES_FOR_FUNCTION_CALL, QSet<CellTypeChooserType>(), m_helper, this, m_owner);
                {
                int localW = 0;
                grp->addToGroup(op_par->render(tx, cy, localW, h));
                tx += localW; op_par->boundingRect().width();
                }

                DatabaseFunctionInstantiationComponent* finstant = owner->getFunctionInstantiationAt(i);
                for(int j=0; j<func->getParameterCount(); j++)
                {
                    SingleExpressionQueryComponent* instParJ = finstant->getInstantiatedParameter(j);
                    QueryGraphicsItem* tItem = instParJ->createGraphicsItem(m_helper, this);
                    {
                    int localW = 0;
                    grp->addToGroup(tItem->render(tx, y, localW, h));
                    tx += localW;
                    }
                }

                // the closing paranthesis
                CellTypeChooser* cl_par = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_CLOSE_PARANTHESES_FOR_FUNCTION_CALL, QSet<CellTypeChooserType>(), m_helper, this, m_owner);
                {
                int localW = 0;
                grp->addToGroup(cl_par->render(tx, cy, localW, h));
                tx += localW;
                }
            }

            grp->addToGroup(bigTypeModifier);
            m_bigTypeModifiers.append(bigTypeModifier);
        }

        // the cursor after the expression
        CellTypeChooser* cursor = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_CURSOR, QSet<CellTypeChooserType>(), m_helper, this, m_owner, funcsAndOperators.size());
        {
        int localW = 0;
        grp->addToGroup(cursor->render(tx, cy, localW, h));
        tx += localW;
        }
        sw += tx - x;

    }

    w = sw;
    if(m_level != -2) y += CELL_SIZE*2;
    h += y - cy - 2;

    m_saveW = w;
    w = saveW>0?saveW:w;
    return grp;
}

void CellForSingleExpression::updateWidth(int newWidth)
{
    if(m_frame)
    {
        QRect t1 = m_frame->rect().toRect();
        t1.setWidth(m_saveW);
        m_frame->setRect(t1);
    }

    if(m_close)
    {
        m_close->updateWidth(m_saveW + (m_level)*CELL_SIZE + 3);//m_frame->boundingRect().right() - (m_level+1)*CELL_SIZE);
    }
}
