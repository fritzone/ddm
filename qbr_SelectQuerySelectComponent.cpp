#include "qbr_SelectQuerySelectComponent.h"
#include "qbr_SelectQuery.h"
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
