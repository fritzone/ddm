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
    const int sx = x, sh = h;// these are saving the input values
    int sw = w;
    int cx = 20;
    int cy = ly + 2;        // two pixels below the top, this is used only for rendering, it's not modified
    int extraCounter = 0;   // counts how many extra ct choosers were added ... (such as paranthese, commas, etc)
    int i=0;
    int localXDepl = 0;

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
        int localW = w;
        QVector<CellTypeChooserType> funcsAndOperators = owner->getFunctionsAndOperators();
        for(i = 0; i<funcsAndOperators.size(); i++)
        {
            QSet<CellTypeChooserType> allowedTypesforBigOne;
            allowedTypesforBigOne.insert(CELLTYPE_REMOVE_THIS);

            CellTypeChooser* bigTypeModifier = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, funcsAndOperators.at(i), allowedTypesforBigOne, m_helper, this, m_owner, i);
            bigTypeModifier->setFunction(owner->getFunctionAt(i));

            bigTypeModifier->render(tx, cy, localW, h);
            tx += bigTypeModifier->boundingRect().width();
            if(tx > sw - CELL_SIZE) sw = tx;

            if(const DatabaseBuiltinFunction* func = bigTypeModifier->getFunction())
            {
                CellTypeChooser* op_par = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_OPEN_PARANTHESES_FOR_FUNCTION_CALL, QSet<CellTypeChooserType>(), m_helper, this, m_owner);
                grp->addToGroup(op_par->render(tx, cy, localW, h));
                tx += op_par->boundingRect().width();
                if(tx > sw - CELL_SIZE) sw = tx;

                DatabaseFunctionInstantiationComponent* finstant = owner->getFunctionInstantiationAt(i);
                for(int j=0; j<func->getParameterCount(); j++)
                {
                    SingleExpressionQueryComponent* instParJ = finstant->getInstantiatedParameter(j);
                    QueryGraphicsItem* tItem = instParJ->createGraphicsItem(m_helper, this);
                    grp->addToGroup(tItem->render(tx, y, localW, h));
                    tx += tItem->boundingRect().width();
                    if(tx > sw - CELL_SIZE) sw = tx;
                }

                // the closing paranthesis
                CellTypeChooser* cl_par = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_CLOSE_PARANTHESES_FOR_FUNCTION_CALL, QSet<CellTypeChooserType>(), m_helper, this, m_owner);

                cl_par->render(tx, cy, localW, h);
                tx += cl_par->boundingRect().width();
                if(tx > sw - CELL_SIZE) sw = tx;
            }

            grp->addToGroup(bigTypeModifier);
            m_bigTypeModifiers.append(bigTypeModifier);
        }

        // the cursor after the expression
        CellTypeChooser* cursor = new CellTypeChooser(m_level, CellTypeChooser::CELLTYPECHOOSER_BIG, CELLTYPE_CURSOR, QSet<CellTypeChooserType>(), m_helper, this, m_owner, i);
        grp->addToGroup(cursor->render(tx, cy, w, h));
    }

    sw += 64 + (extraCounter + i + 1) * CELL_SIZE + localXDepl;

    x = sx; w = sw; h = sh;

    y += CELL_SIZE*2+4;

    y+=2;
    h += y - cy - 2;

    m_saveW = w;
    w = saveW;
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
