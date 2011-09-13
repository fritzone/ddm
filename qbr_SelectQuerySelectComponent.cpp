#include "qbr_SelectQuerySelectComponent.h"
#include "qbr_SelectQuery.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "strings.h"

SelectQuerySelectComponent::SelectQuerySelectComponent(QueryComponent* p, int l) : QueryComponent(p,l)
{
}

QSet<OptionsType> SelectQuerySelectComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_WHERE_EXPR);
    return t;
}

void SelectQuerySelectComponent::onClose()
{
    if(m_parent)
    {
        m_parent->onClose();
    }
}

QueryComponent* SelectQuerySelectComponent::duplicate()
{
    SelectQuerySelectComponent* newc = new SelectQuerySelectComponent(m_parent, m_level);
    return newc;
}

void SelectQuerySelectComponent::handleAction(const QString& action, QueryComponent* referringObject)
{
    if(action == ADD_WHERE_EXPRESSION)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            sq->newSelectExpression();
        }
    }
}

bool SelectQuerySelectComponent::hasGroupByFunctions()
{
    for(int i=0; i<m_children.size(); i++)
    {
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_children.at(i));
        if(seq)
        {
            if(seq->hasGroupByFunctions()) return true;
        }
    }
    return false;
}

bool SelectQuerySelectComponent::hasAtLeastOneColumnSelected()
{
    for(int i=0; i<m_children.size(); i++)
    {
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_children.at(i));
        if(seq)
        {
            if(seq->hasAtLeastOneColumnSelected()) return true;
        }
    }
    return false;
}

QVector<const Column*> SelectQuerySelectComponent::getSelectedColumns()
{
    QVector<const Column*> result;
    for(int i=0; i<m_children.size(); i++)
    {
        QVector<const Column*> localResult;
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_children.at(i));
        if(seq)
        {
            if(seq->hasAtLeastOneColumnSelected())
            {
                localResult = seq->getColumns();
                result += localResult;
            }
        }
    }
    return result;
}
