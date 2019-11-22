#include "qbr_SelectQueryWhereComponent.h"
#include "qbr_OptionTypes.h"
#include "qbr_SelectQuery.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "uids.h"

#include "ddm_strings.h"

SelectQueryWhereComponent::SelectQueryWhereComponent(Query* q, QueryComponent* p,
                                                     int l, WhereType w,
                                                     Version* v) :
    QueryComponent(q, p, l, v),
    m_whereType(w)
{
}

void SelectQueryWhereComponent::handleAction(const QString &action, QueryComponent* /*referringObject*/)
{
    if(action == ADD_WHERE_EXPRESSION)
    {
        SelectQuery* sq = dynamic_cast<SelectQuery*>(getParent());
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
        SelectQuery* sq = dynamic_cast<SelectQuery*>(getParent());
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
        SelectQuery* sq = dynamic_cast<SelectQuery*>(getParent());
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
    SelectQueryWhereComponent* newc =
            new SelectQueryWhereComponent(getQuery(), getParent(), getLevel(),
                                          m_whereType, version());
    return newc;
}

CloneableElement* SelectQueryWhereComponent::clone(Version* sourceVersion,
                                                   Version* targetVersion)
{
    SelectQueryWhereComponent* nc = dynamic_cast<SelectQueryWhereComponent*>(duplicate());
    nc->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, nc);
    return nc;
}


QString SelectQueryWhereComponent::get() const
{
    QString result = "";
    switch(m_whereType)
    {
    case WHERETYPE_WHERE:
        result = "WHERE";
        break;
    case WHERETYPE_HAVING:
        result = "HAVING";
        break;
    case WHERETYPE_ON:
        result = "ON";
        break;
    }

    if(hasChildren())
    {
        result += strNewline;
    }

    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        result += getSpacesForLevel();
        result+= children.at(i)->get();
        SingleExpressionQueryComponent* cI = dynamic_cast<SingleExpressionQueryComponent*>(children.at(i));
        if(cI)
        {
            if((i == 0 || i == children.size()-1) && cI->hasForcedType())
            {
                result += QString(" (") + QString("?") + QString("?") + QString(")"); // first/last should not be AND or OR
            }
            if(i>0)
            {
                // if this is NOT a forced type before this there should be
                if(!cI->hasForcedType())
                {
                    if(! (dynamic_cast<SingleExpressionQueryComponent*>(children.at(i-1))->hasForcedType()))
                    {
                        result += QString(" (") + QString("?") + QString("?") + QString(")");
                    }
                }
            }
        }
        result += "\n";
    }
    return result;
}

void SelectQueryWhereComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QString nodeName = "";
    switch(m_whereType)
    {
    case WHERETYPE_WHERE:
        nodeName = "WhereComponent";
        break;
    case WHERETYPE_HAVING:
        nodeName = "HavingComponent";
        break;
    case WHERETYPE_ON:
        nodeName = "OnComponent";
        break;
    }
    QDomElement whereElement = doc.createElement(nodeName);
    whereElement.setAttribute("Type", m_whereType);
    QueryComponent::serialize(doc, whereElement);
    parent.appendChild(whereElement);
}

QUuid SelectQueryWhereComponent::getClassUid() const
{
    return QUuid(uidSelectQueryWhereComponent);
}
