#include "qbr_SelectQueryGroupByComponent.h"
#include "qbr_SelectQuery.h"
#include "strings.h"

SelectQueryGroupByComponent::SelectQueryGroupByComponent(QueryComponent* p, int l):QueryComponent(p,l)
{
}


void SelectQueryGroupByComponent::handleAction(const QString &action, QueryComponent* referringObject)
{
    if(action == NEW_COLUMN)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(m_parent);
        if(sq)
        {
            sq->newGroupByExpression();
        }
    }
}

QSet<OptionsType> SelectQueryGroupByComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_COLUMN);
    return t;
}

QueryComponent* SelectQueryGroupByComponent::duplicate()
{
    SelectQueryGroupByComponent *newc = new SelectQueryGroupByComponent(m_parent, m_level);
    return newc;
}

QString SelectQueryGroupByComponent::get() const
{
    QString result = "GROUP BY";
    if(m_children.size()) result += "\n";
    for(int i=0; i<m_children.size(); i++)
    {
        result += getSpacesForLevel();
        result+= m_children.at(i)->get();
    }
    return result;
}

void SelectQueryGroupByComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement groupBy = doc.createElement("GroupBy");
    QueryComponent::serialize(doc, groupBy);
    parent.appendChild(groupBy);
}

