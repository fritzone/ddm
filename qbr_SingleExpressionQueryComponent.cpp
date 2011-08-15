#include "qbr_SingleExpressionQueryComponent.h"
#include "CellForUnaryWhereExpression.h"
#include "Workspace.h"
#include "DatabaseEngine.h"

#include <QDebug>

SingleExpressionQueryComponent::SingleExpressionQueryComponent(QueryComponent* p, int l): WhereExpressionQueryComponent(p,l), m_functionsAndOperators()
{
}

QueryGraphicsItem* SingleExpressionQueryComponent::createGraphicsItem(QueryGraphicsHelper * helper, QueryGraphicsItem * parent)
{
    m_helper = helper;
    m_gritm = new CellForUnaryWhereExpression(getLevel() + 1, helper, parent, this);

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
    for(int i=after+1; i<m_functionsAndOperators.size(); i++)
    {
        if(m_functionsAtGivenPosition.contains(i))
        {
            m_functionsAtGivenPosition.insert(i-1, m_functionsAtGivenPosition.value(i));
            m_functionsAtGivenPosition.remove(i);
        }
    }
}

void SingleExpressionQueryComponent::handleAction(const QString& action, QueryComponent* referringObject)
{
    if(action == "NOT")
    {
        m_functionsAndOperators.append(CELLTYPE_NOT);
        m_helper->triggerReRender();
        return;
    }
    if(action == "NEG")
    {
        m_functionsAndOperators.append(CELLTYPE_NEGATE);
        m_helper->triggerReRender();
        return;
    }
    if(action == "MINUS")
    {
        m_functionsAndOperators.append(CELLTYPE_MINUS);
        m_helper->triggerReRender();
        return;
    }
    if(action.startsWith("REMOVE"))
    {
        int idx = action.right(action.length() - 7).toInt(); // because actually REMOVE_xxx comes in
        m_functionsAndOperators.remove(idx);
        shiftFunctionsToTheLeft(idx);
        m_helper->triggerReRender();
        return;
    }
    if(action == "Function")
    {
        m_functionsAndOperators.append(CELLTYPE_FUNCTION);
        m_helper->triggerReRender();
        return;
    }

    if(action.startsWith("@"))
    {
        // this is a function...
        qDebug() << "Function: "<< action;
        QString fName = action.mid(1, action.indexOf('_') - 1);
        int index = action.section('_', 1).toInt();
         m_functionsAndOperators.append(CELLTYPE_FUNCTION);
        if(m_functionsAtGivenPosition.find(index) == m_functionsAtGivenPosition.end())
        {
            m_functionsAtGivenPosition.insert(index, Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName));
        }
        else
        {
            m_functionsAtGivenPosition[index] = Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunction(fName);
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
    QVector<CellTypeChooserType> allowedTypes;
//    allowedTypes.append(CELLTYPE_NOTHING);
//    allowedTypes.append(CELLTYPE_NEGATE);
//    allowedTypes.append(CELLTYPE_MINUS);
//    allowedTypes.append(CELLTYPE_NOT);
//    allowedTypes.append(CELLTYPE_FUNCTION);
//    allowedTypes.append(CELLTYPE_LITERAL);

    return allowedTypes;
}
