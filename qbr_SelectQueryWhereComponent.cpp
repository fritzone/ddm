#include "qbr_SelectQueryWhereComponent.h"
#include "qbr_OptionTypes.h"
#include "qbr_SelectQuery.h"

#include "strings.h"


SelectQueryWhereComponent::SelectQueryWhereComponent(QueryComponent* p, int l, WhereType w):QueryComponent(p,l), m_whereType(w)
{
}


void SelectQueryWhereComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
    if(action == ADD_WHERE_EXPRESSION)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            if(m_whereType == WHERETYPE_HAVING)
            {
                sq->newHavingExpression();
            }
            else
            {
                sq->newWhereExpression();
            }
        }
    }

    if(action == ADD_WHERE_EXPRESSION_OR)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            if(m_whereType == WHERETYPE_HAVING)
            {
                sq->newHavingExpression(SelectQuery::OR_NEW_WHERE_EXPRESSION);
            }
            else
            {
                sq->newWhereExpression(SelectQuery::OR_NEW_WHERE_EXPRESSION);
            }
        }
    }

    if(action == ADD_WHERE_EXPRESSION_AND)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            if(m_whereType == WHERETYPE_HAVING)
            {
                sq->newHavingExpression(SelectQuery::OR_NEW_WHERE_EXPRESSION);
            }
            else
            {
                sq->newWhereExpression(SelectQuery::AND_NEW_WHERE_EXPRESSION);
            }
        }
    }
}

QSet<OptionsType> SelectQueryWhereComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_WHERE_EXPR_OR);
    t.insert(OPTIONS_NEW_WHERE_EXPR_AND);
    t.insert(OPTIONS_NEW_WHERE_EXPR);
    return t;
}

QueryComponent* SelectQueryWhereComponent::duplicate()
{
    SelectQueryWhereComponent* newc = new SelectQueryWhereComponent(m_parent, m_level, m_whereType);
    return newc;
}
