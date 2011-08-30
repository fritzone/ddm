#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_CellForSingleExpression.h"
#include "Workspace.h"
#include "DatabaseEngine.h"
#include "DatabaseBuiltinFunction.h"
#include "qbr_DatabaseFunctionInstantiationComponent.h"

#include <QDebug>

SingleExpressionQueryComponent::SingleExpressionQueryComponent(QueryComponent* p, int l): WhereExpressionQueryComponent(p,l),
    m_gritm(0), m_elements(), m_columnsAtGivenPosition(), m_functionsAtGivenPosition(), m_functionInstantiationAtGivenPosition()
{
}

QueryGraphicsItem* SingleExpressionQueryComponent::createGraphicsItem(QueryGraphicsHelper * helper, QueryGraphicsItem * parent)
{
    m_helper = helper;
    int l = getLevel();
    int adder = 1;
    if(l == -2) adder = 0;
    m_gritm = new CellForSingleExpression(l + adder, helper, parent, this);

    return m_gritm;

}

QString SingleExpressionQueryComponent::get()
{
    return "Implement this";
}

QSet<OptionsType> SingleExpressionQueryComponent::provideOptions()
{
    QSet<OptionsType> t;
    return t;
}

QueryComponent* SingleExpressionQueryComponent::duplicate()
{
    SingleExpressionQueryComponent* newc = new SingleExpressionQueryComponent(m_parent, m_level);
    return newc;
}

void SingleExpressionQueryComponent::shiftFunctionsToTheLeft(int after)
{
    for(int i=after+1; i<m_elements.size(); i++)
    {
        if(m_functionsAtGivenPosition.contains(i))
        {
            m_functionsAtGivenPosition.insert(i-1, m_functionsAtGivenPosition.value(i));
            m_functionsAtGivenPosition.remove(i);
        }

        if(m_functionInstantiationAtGivenPosition.contains(i))
        {
            m_functionInstantiationAtGivenPosition.insert(i-1, m_functionInstantiationAtGivenPosition.value(i));
            m_functionInstantiationAtGivenPosition.remove(i);
        }
    }
}

const DatabaseBuiltinFunction* SingleExpressionQueryComponent::getFunctionAt(int i)
{
    if(m_functionsAtGivenPosition.contains(i)) return m_functionsAtGivenPosition[i];
    return 0;
}


DatabaseFunctionInstantiationComponent* SingleExpressionQueryComponent::getFunctionInstantiationAt(int pos)
{
    if(m_functionInstantiationAtGivenPosition.contains(pos)) return m_functionInstantiationAtGivenPosition[pos];
    return 0;
}

void SingleExpressionQueryComponent::handleAction(const QString& action, QueryComponent* referringObject)
{
    QMap<QString,CellTypeChooserType> mappings;
    mappings[strMathMinus] = CELLTYPE_MINUS;
    mappings[strMathPlus] = CELLTYPE_PLUS;
    mappings[strMathDivide] = CELLTYPE_DIVIDE;
    mappings[strMathMultiply] = CELLTYPE_MULTIPLY;
    mappings[strMathMod] = CELLTYPE_MOD;

    mappings[strLogNeg] = CELLTYPE_NEGATE;
    mappings[strLogNot] = CELLTYPE_NOT;
    mappings[strLogOr] = CELLTYPE_OR;
    mappings[strLogAnd] = CELLTYPE_AND;
    mappings[strLogXor] = CELLTYPE_XOR;
    mappings[strLogLShift] = CELLTYPE_LSHIFT;
    mappings[strLogRShift] = CELLTYPE_RSHIFT;

    mappings[strCmpEqual] = CELLTYPE_EQUAL;
    mappings[strCmpNotEqual] = CELLTYPE_NOTEQUAL;
    mappings[strCmpLess] = CELLTYPE_LESS;
    mappings[strCmpGreater] = CELLTYPE_GREATER;
    mappings[strCmpLessOrEqual] = CELLTYPE_LESS_OR_EQUAL;
    mappings[strCmpGreaterOrEqual] = CELLTYPE_GREATER_OR_EQUAL;

    if(action.indexOf(strActionIndexSeparator)&& !action.startsWith(strRemove)&&!action.startsWith("@"))
    {
        QString pureAction = action.left(action.indexOf(strActionIndexSeparator));
        if(mappings.contains(pureAction))
        {
            int indx = action.section(strActionIndexSeparator, 1).toInt();
            if(indx < m_elements.size())
            {
                m_elements[indx] = mappings[pureAction];
                if(m_functionsAtGivenPosition.find(indx) != m_functionsAtGivenPosition.end())
                {
                    m_functionInstantiationAtGivenPosition.remove(indx);
                    m_functionsAtGivenPosition.remove(indx);
                }
            }
            else
            {
                m_elements.append(mappings[pureAction]);
            }
        }
        m_helper->triggerReRender();
        return;
    }

    if(action.startsWith(strRemove) && !m_elements.isEmpty())
    {
        int idx = action.right(action.length() - 7).toInt(); // because actually REMOVE^xxx comes in
        m_elements.remove(idx);
        shiftFunctionsToTheLeft(idx);
        m_helper->triggerReRender();
        return;
    }

    if(action.startsWith("@"))          // this is a function...
    {
        qDebug() << "Function: "<< action;
        QString fName = action.mid(1, action.indexOf(strActionIndexSeparator) - 1);
        int index = action.section(strActionIndexSeparator, 1).toInt();
        m_elements.append(CELLTYPE_FUNCTION);
        if(m_functionsAtGivenPosition.find(index) == m_functionsAtGivenPosition.end())
        {
            m_functionsAtGivenPosition.insert(index, &Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName));
            m_functionInstantiationAtGivenPosition.insert(index, new DatabaseFunctionInstantiationComponent(new SingleExpressionQueryComponent(this, m_level + 1), Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName)));
        }
        else
        {
            m_functionsAtGivenPosition[index] = &Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName);
            m_functionInstantiationAtGivenPosition[index] = new DatabaseFunctionInstantiationComponent(new SingleExpressionQueryComponent(this, m_level + 1), Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName));
        }
        m_helper->triggerReRender();
    }

    if(action.startsWith("#"))  // This is a table->column at given position
    {

    }
}

bool SingleExpressionQueryComponent::hasFunctionAtIndex(int i)
{
    return m_functionsAtGivenPosition.find(i) != m_functionsAtGivenPosition.end();
}

QVector<CellTypeChooserType> SingleExpressionQueryComponent::getChoosableTypes() const
{
    QVector<CellTypeChooserType> allowedTypes;
//    allowedTypes.append(CELLTYPE_NOTHING);
//    allowedTypes.append(CELLTYPE_NEGATE);
//    allowedTypes.append(CELLTYPE_MINUS);
//    allowedTypes.append(CELLTYPE_NOT);
//    allowedTypes.append(CELLTYPE_FUNCTION);
//    allowedTypes.append(CELLTYPE_LITERAL);

    return allowedTypes;
}
