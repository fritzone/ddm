#include "SelectQueryWhereComponent.h"
#include "OptionsType.h"
#include "SelectQuery.h"

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
            sq->newWhereExpression();
        }
    }
}

QSet<OptionsType> SelectQueryWhereComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_WHERE_EXPR);
    return t;
}

QueryComponent* SelectQueryWhereComponent::duplicate()
{
    SelectQueryWhereComponent* newc = new SelectQueryWhereComponent(m_parent, m_level, m_whereType);
    return newc;
}
