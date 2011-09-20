#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_CellForSingleExpression.h"
#include "Workspace.h"
#include "DatabaseEngine.h"
#include "DatabaseBuiltinFunction.h"
#include "qbr_DatabaseFunctionInstantiationComponent.h"
#include "Table.h"
#include "Column.h"
#include "Version.h"
#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_WhereExpressionQueryComponent.h"
#include "qbr_SelectQuerySelectComponent.h"
#include "qbr_CellAsCommand.h"
#include "qbr_SelectQueryAsComponent.h"

#include <QDebug>

SingleExpressionQueryComponent::SingleExpressionQueryComponent(QueryComponent* p, int l): WhereExpressionQueryComponent(p,l),
    m_gritm(0), m_elements(), m_columnsAtGivenPosition(), m_functionsAtGivenPosition(), m_functionInstantiationAtGivenPosition(), m_typedValuesAtGivenPosition(), m_as(0)
{
}

QueryGraphicsItem* SingleExpressionQueryComponent::createGraphicsItem(QueryGraphicsHelper * helper, QueryGraphicsItem * parent)
{
    m_helper = helper;
    int l = getLevel();
    int adder = 1;
    if(l == -2) adder = 0;
    m_gritm = new CellForSingleExpression(l + adder, helper, parent, this);
    if(m_as)
    {
        m_gritm->setAs(new CellAsCommand(helper, m_level + 1, m_gritm, m_as, true));
    }

    return m_gritm;

}

QString SingleExpressionQueryComponent::get()
{
    return "Implement this";
}

QSet<OptionsType> SingleExpressionQueryComponent::provideOptions()
{
    QSet<OptionsType> t;
    if(dynamic_cast<SelectQuerySelectComponent*>(m_parent))
    {
        t.insert(OPTIONS_AS);
    }
    return t;
}

QueryComponent* SingleExpressionQueryComponent::duplicate()
{
    SingleExpressionQueryComponent* newc = new SingleExpressionQueryComponent(m_parent, m_level);
    return newc;
}

void SingleExpressionQueryComponent::shiftElementsToTheLeft(int after)
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

        if(m_columnsAtGivenPosition.contains(i))
        {
            m_columnsAtGivenPosition.insert(i-1, m_columnsAtGivenPosition.value(i));
            m_columnsAtGivenPosition.remove(i);
        }

        if(m_typedValuesAtGivenPosition.contains(i))
        {
            m_typedValuesAtGivenPosition.insert(i-1, m_typedValuesAtGivenPosition.value(i));
            m_typedValuesAtGivenPosition.remove(i);
        }
    }
}

const DatabaseBuiltinFunction* SingleExpressionQueryComponent::getFunctionAt(int i)
{
    if(m_functionsAtGivenPosition.contains(i)) return m_functionsAtGivenPosition[i];
    return 0;
}

const Column* SingleExpressionQueryComponent::getColumnAt(int i)
{
    if(m_columnsAtGivenPosition.contains(i)) return m_columnsAtGivenPosition[i];
    return 0;
}

const QString SingleExpressionQueryComponent::getTypedInValueAt(int i)
{
    if(m_typedValuesAtGivenPosition.contains(i)) return m_typedValuesAtGivenPosition[i];
    return "";
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

    mappings[strNotText] = CELLTYPE_NOT_TEXT;
    mappings[strLike] = CELLTYPE_LIKE;
    mappings[strIn] = CELLTYPE_IN;
    mappings[strIs] = CELLTYPE_IS;
    mappings[strBetween] = CELLTYPE_BETWEEN;
    mappings[strExists] = CELLTYPE_EXISTS;
    mappings[strDistinct] = CELLTYPE_DISTINCT;
    mappings[strStar] = CELLTYPE_STAR;
    mappings[strRollup] = CELLTYPE_ROLLUP;
    mappings[strNull] = CELLTYPE_NULL;
    mappings[strOpenParantheses] = CELLTYPE_OPEN_PARANTHESES;
    mappings[strCloseParantheses] = CELLTYPE_CLOSE_PARANTHESES;

    if(action.indexOf(strActionIndexSeparator)&& !action.startsWith(strRemove)&&!action.startsWith("@")&&!action.startsWith("#")&&!action.startsWith("$"))
    {
        QString pureAction = action.left(action.indexOf(strActionIndexSeparator));
        if(mappings.contains(pureAction))
        {
            // let's check if this is ROLLUP. There can be only one
            if(mappings[pureAction] == CELLTYPE_ROLLUP)
            {
                for(int i=0; i<m_elements.size(); i++)
                {
                    if(m_elements.at(i) == CELLTYPE_ROLLUP) // only one ROLLUP enabled
                    {
                        return;
                    }
                }
            }
            int indx = action.section(strActionIndexSeparator, 1).toInt();
            if(indx < m_elements.size())
            {
                m_elements[indx] = mappings[pureAction];
                if(m_functionsAtGivenPosition.find(indx) != m_functionsAtGivenPosition.end())   // check if we overwrote a function
                {
                    m_functionInstantiationAtGivenPosition.remove(indx);
                    m_functionsAtGivenPosition.remove(indx);
                }
                if(m_columnsAtGivenPosition.find(indx) != m_columnsAtGivenPosition.end())   // check if we overwrote a column
                {
                    m_columnsAtGivenPosition.remove(indx);
                }

                if(m_typedValuesAtGivenPosition.contains(indx)) // check if we overwrote a typed in value
                {
                    m_typedValuesAtGivenPosition.remove(indx);
                }
            }
            else
            {
                m_elements.append(mappings[pureAction]);
            }
        }
        else
        if(action == ADD_ALIAS)
        {
            m_as = new SelectQueryAsComponent(this, m_level + 1);
            addChild(m_as);
            m_helper->triggerReRender();
        }
        m_helper->triggerReRender();
        return;
    }

    if(action.startsWith(strRemove) && !m_elements.isEmpty())
    {
        int idx = action.right(action.length() - 7).toInt(); // because actually REMOVE^xxx comes in
        m_elements.remove(idx);
        shiftElementsToTheLeft(idx);
        m_helper->triggerReRender();
        return;
    }

    if(action.startsWith("@"))          // this is a function...
    {
        QString fName = action.mid(1, action.indexOf(strActionIndexSeparator) - 1);
        int index = action.section(strActionIndexSeparator, 1).toInt();
        if(index < m_elements.size())   // let's see if we overwrote something
        {
            m_elements[index] = CELLTYPE_FUNCTION;
        }
        else
        {
            m_elements.append(CELLTYPE_FUNCTION);
        }

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

        // let's see if we overwrote a column
        if(m_columnsAtGivenPosition.contains(index))
        {
            m_columnsAtGivenPosition.remove(index);
        }

        // or a typed in value
        if(m_typedValuesAtGivenPosition.contains(index))
        {
            m_typedValuesAtGivenPosition.remove(index);
        }
        m_helper->triggerReRender();
    }

    if(action.startsWith("#"))  // This is a table->column at given position: "#PERSON+NAME^0"
    {
        QStringList t = action.split('+');
        QString t1 = t.at(0);
        QString tabName = t1.right(t1.length() - 1);
        t1 = t.at(1);
        QStringList t2 = t1.split('^');
        QString colName = t2.at(0);
        int index = t2.at(1).toInt();
        Table* tab = Workspace::getInstance()->workingVersion()->getTable(tabName);
        if(tab == 0) { qDebug() << "No table???"; return; }
        Column* c = tab->getColumn(colName);
        if(c == 0) c = tab->getColumnFromParents(colName);
        if(c == 0) { qDebug() << "No column???"; return; }
        m_columnsAtGivenPosition[index] = c;

        if(index < m_elements.size())   // let's see if we overwrote something
        {
            m_elements[index] = CELLTYPE_COLUMN;
        }
        else
        {
            m_elements.append(CELLTYPE_COLUMN);
        }

        if(m_functionsAtGivenPosition.find(index) != m_functionsAtGivenPosition.end())  // we had a function at this position
        {
            m_functionsAtGivenPosition.remove(index);
            m_functionInstantiationAtGivenPosition.remove(index);
        }
        // or a typed in value
        if(m_typedValuesAtGivenPosition.contains(index))
        {
            m_typedValuesAtGivenPosition.remove(index);
        }
        m_helper->triggerReRender();
    }

    if(action.startsWith("$"))  // this is a typed in value
    {
        int t = action.lastIndexOf(strActionIndexSeparator);
        QString typedText = action.mid(1, t-1);
        QString sIdx = action.right(action.length() - t - 1);
        int index = sIdx.toInt();
        m_typedValuesAtGivenPosition[index] = typedText;
        if(index < m_elements.size())   // let's see if we overwrote something
        {
            m_elements[index] = CELLTYPE_LITERAL;
        }
        else
        {
            m_elements.append(CELLTYPE_LITERAL);
        }
        if(m_functionsAtGivenPosition.find(index) != m_functionsAtGivenPosition.end())  // we had a function at this position
        {
            m_functionsAtGivenPosition.remove(index);
            m_functionInstantiationAtGivenPosition.remove(index);
        }
        // let's see if we overwrote a column
        if(m_columnsAtGivenPosition.contains(index))
        {
            m_columnsAtGivenPosition.remove(index);
        }
        m_helper->triggerReRender();

    }
}

bool SingleExpressionQueryComponent::hasFunctionAtIndex(int i)
{
    return m_functionsAtGivenPosition.find(i) != m_functionsAtGivenPosition.end();
}

QVector<CellTypeChooserType> SingleExpressionQueryComponent::getChoosableTypes() const
{
    return QVector<CellTypeChooserType> ();
}

void SingleExpressionQueryComponent::setForcedType(ForcedSingleExpressionQueryComponent t)
{
    m_forcedType = t;
    if(m_forcedType == FORCED_OR)
    {
        m_elements.append(CELLTYPE_QUERY_OR);
    }
    if(m_forcedType == FORCED_AND)
    {
        m_elements.append(CELLTYPE_QUERY_AND);
    }
}

void SingleExpressionQueryComponent::removeFromOn()
{
    m_onOwner->removeExpression(m_onComponent);
}

void SingleExpressionQueryComponent::onClose()
{
    m_parent->removeChild(this);
    if(m_ownedByOn)
    {
        removeFromOn();
    }
    m_helper->triggerReRender();
}

bool SingleExpressionQueryComponent::hasGroupByFunctions()
{
    QMap<int, const DatabaseBuiltinFunction*>::const_iterator it = m_functionsAtGivenPosition.constBegin();
    while (it != m_functionsAtGivenPosition.constEnd())
    {
        if(it.value()->getType() == FT_AGGREGATE)
        {
            return true;
        }
        ++it;
    }
    return false;
}

bool SingleExpressionQueryComponent::hasAtLeastOneColumnSelected()
{
    for(int i=0; i<m_elements.size(); i++)
    {
        if(m_elements.at(i) == CELLTYPE_COLUMN) return true;
    }
    return false;
}

QVector<const Column*> SingleExpressionQueryComponent::getColumns()
{
    QVector<const Column*> result;
    QMap<int, const Column*>::const_iterator it = m_columnsAtGivenPosition.begin();
    while(it != m_columnsAtGivenPosition.end())
    {
        result.push_back(it.value());
        ++ it;
    }
    return result;
}

void SingleExpressionQueryComponent::removeAs()
{
    m_as = 0;
    m_helper->triggerReRender();
}

const SelectQueryAsComponent* SingleExpressionQueryComponent::hasAs()
{
    return m_as;
}
