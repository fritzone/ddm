#include "qbr_CellClose.h"
#include "qbr_CellForSingleExpression.h"
#include "qbr_CellTypeChooser.h"
#include "qbr_OptionTypes.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_DatabaseFunctionInstantiationComponent.h"
#include "qbr_QueryGraphicsItem.h"
#include "qbr_CellAsCommand.h"
#include "qbr_CellQuerySmallOptionsBox.h"

#include <QBrush>

CellForSingleExpression::CellForSingleExpression (int level, QueryGraphicsHelper *c, QueryGraphicsItem *parent, QueryComponent *owner):
    QueryGraphicsItem(level, parent, c, owner), m_frame(0), m_close(0), m_smallTypeModifier(0), m_as(0)
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
        m_close->render(cx, y, w, h);
        grp->addToGroup(m_close);
        m_close->setZValue(2);
    }

    if(SingleExpressionQueryComponent* owner = dynamic_cast<SingleExpressionQueryComponent*>(m_owner))
    {
        int tx = x + 2; // this holds where we are putting the next cell type chooser
        int i = 0;
        QVector<CellTypeChooserType> elements = owner->getElements();
        for(i = 0; i<elements.size(); i++)
        {
            CellTypeChooser* bigTypeModifier = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, elements.at(i), QSet<CellTypeChooserType>(), m_helper, this, m_owner, i);
            bigTypeModifier->setFunction(owner->getFunctionAt(i));  // these might or might not set
            bigTypeModifier->setColumn(owner->getColumnAt(i));
            bigTypeModifier->setLiteral(owner->getTypedInValueAt(i));

            {
            int localW = 0;
            bigTypeModifier->render(tx, cy, localW, h);
            tx += localW; //bigTypeModifier->boundingRect().width();
            }

            if(const DatabaseBuiltinFunction* func = bigTypeModifier->getFunction())    // let's see if we have function here and in this case render the parameters of the function
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
            tx ++;
        }

        // the cursor after the expression, but only if the last element is not a QUERY OR aor a QUERY AND
        if(elements.size() == 0 || (elements.at(i-1) != CELLTYPE_QUERY_OR && elements.at(i-1) != CELLTYPE_QUERY_AND))
        {
            CellTypeChooser* cursor = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_CURSOR, QSet<CellTypeChooserType>(), m_helper, this, m_owner, elements.size());
            int oldTx = tx;
            {
            int localW = 0;
            grp->addToGroup(cursor->render(tx, cy, localW, h));
            tx += localW;
            }
            sw = tx - x;
            sw += 15;
        }

    }

    w = sw;
    if(m_level != -2) y += m_frame->boundingRect().height() + 4;
    m_saveW = w;
    w = saveW>0?saveW:w;

    if(m_as)
    {
        int lmw = sw;
        x += CHILDREN_ALIGNMENT;
        int oldy = y-2;
        int neww = lmw - (m_as->getLevel() + 1)* 20;
        grp->addToGroup(m_as->render(x, y, neww, h));
        int halfway = (oldy + y) / 2 - 5;
        if(w<neww-20) {w = neww - 20; lmw = neww - 20;}

        grp->addToGroup(new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , oldy - 1, x + 5+2-CHILDREN_ALIGNMENT, halfway , this));  // top
        //if(!m_joins.isEmpty())
        //{
        //grp->addToGroup(    new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT , halfway +10, x + 5+2-CHILDREN_ALIGNMENT, y, this));   // botton
        //}
        grp->addToGroup(new QGraphicsLineItem(x +5+2-CHILDREN_ALIGNMENT +5 , halfway+5, x + 1, halfway+5, this));                    // to right

        // this will be the small options box before the AS
        QSet<OptionsType> t;
        QSet<OptionsType> more = m_as->getOwner()->provideOptions();
        t.unite(more);

        CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBox(t, m_helper, m_as->getLevel(), m_parent, m_as->getOwner(), CellQuerySmallOptionsBox::SHAPE_DIAMOND);
        int tx = x-15 + 2; int ty = halfway; int tw = w; int th = h;
        grp->addToGroup(smb->render(tx, ty, tw, th));

        x -= CHILDREN_ALIGNMENT;
        w = w<lmw?lmw:w;

        y+=2;

    }

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
        m_close->updateWidth(m_frame->rect().right() - 20); //m_saveW + (m_level)*CELL_SIZE + 3);
    }

    if(m_as)
    {
        m_as->updateWidth(newWidth);
    }
}

void CellForSingleExpression::onClose()
{
    m_owner->onClose();
}
