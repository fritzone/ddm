#include "qbr_SelectQueryOrderByComponent.h"
#include "qbr_SelectQuery.h"

#include "strings.h"

#include <QDebug>

SelectQueryOrderByComponent::SelectQueryOrderByComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
}


void SelectQueryOrderByComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
    qDebug() << action;
    if(action == NEW_COLUMN)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            sq->newOrderByExpression();
        }
    }
}

QSet<OptionsType> SelectQueryOrderByComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_COLUMN);
    return t;
}

QueryComponent* SelectQueryOrderByComponent::duplicate()
{
    SelectQueryOrderByComponent* newc = new SelectQueryOrderByComponent(m_parent, m_level);
    return newc;
}
