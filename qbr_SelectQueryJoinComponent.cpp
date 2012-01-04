#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_SingleExpressionQueryComponent.h"

#include "strings.h"

SelectQueryJoinComponent::SelectQueryJoinComponent(QueryComponent* p, int l):QueryComponent(p,l),  m_helper(0), m_joinExpressions()
{
}

void SelectQueryJoinComponent::handleAction(const QString& action, QueryComponent* /*referringObject*/)
{
    if(action == NEW_TABLE)
    {
        TableQueryComponent* tccp = TableQueryComponent::provideFirstTableIfAny(this, m_level);
        if(!tccp) return;
        addChild(tccp);
        m_helper->triggerReRender();
        return;
    }
    if(action == ADD_WHERE_EXPRESSION)  // we get here when the user clicked the New join expr...
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(this, m_level);
        m_joinExpressions.append(c);
        m_helper->triggerReRender();
        return;
    }
    if(action == ADD_WHERE_EXPRESSION_AND)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(this, m_level);
        dynamic_cast<SingleExpressionQueryComponent*>(c)->setForcedType(SingleExpressionQueryComponent::FORCED_AND);
        m_joinExpressions.append(c);
        m_helper->triggerReRender();
        return;
    }
    if(action == ADD_WHERE_EXPRESSION_OR)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(this, m_level);
        dynamic_cast<SingleExpressionQueryComponent*>(c)->setForcedType(SingleExpressionQueryComponent::FORCED_OR);
        m_joinExpressions.append(c);
        m_helper->triggerReRender();
        return;
    }
}

QueryGraphicsItem* SelectQueryJoinComponent::createGraphicsItem(QueryGraphicsHelper* helper, QueryGraphicsItem*)
{
    m_helper = helper;
    return 0;
}

QSet<OptionsType> SelectQueryJoinComponent::provideOptions()
{
    QSet<OptionsType> t;
    t.insert(OPTIONS_NEW_TABLE);
    return t;
}

void SelectQueryJoinComponent::onClose()
{
    TableQueryComponent* parent = dynamic_cast<TableQueryComponent*>(m_parent);
    if(parent)
    {
        parent->removeJoin(this);
        m_helper->triggerReRender();
    }
}

QueryComponent* SelectQueryJoinComponent::duplicate()
{
    SelectQueryJoinComponent* newc = new SelectQueryJoinComponent(m_parent, m_level);
    for(int i=0; i<m_joinExpressions.size(); i++)
    {
        newc->m_joinExpressions.append(dynamic_cast<SingleExpressionQueryComponent*>(m_joinExpressions.at(i)->duplicate()));
    }
    return newc;
}

void SelectQueryJoinComponent::removeExpression(SingleExpressionQueryComponent *w)
{
    m_joinExpressions.remove(m_joinExpressions.indexOf(w));
}

QString SelectQueryJoinComponent::get() const
{
    QString result = getSpacesForLevel() + "JOIN";
    for(int i=0; i<m_children.size(); i++)
    {
        result += getSpacesForLevel();
        result+= m_children.at(i)->get();
    }
    result += "\n" + getSpacesForLevel() + " ON";
    for(int i=0; i<m_joinExpressions.size(); i++)
    {
        result += "\n" + getSpacesForLevel();
        result += m_joinExpressions.at(i)->get();
    }
    return result;
}

void SelectQueryJoinComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement joinElement = doc.createElement("Join");
    QueryComponent::serialize(doc, joinElement);    // this has put the children
    QDomElement onElement = doc.createElement("On");
    for(int i=0; i<m_joinExpressions.size(); i++)
    {
        QDomElement joinExpression = doc.createElement("JoinExpression");
        joinExpression.setAttribute("idx", i);
        m_joinExpressions.at(i)->serialize(doc, joinExpression);
        onElement.appendChild(joinExpression);
    }
    joinElement.appendChild(onElement);
    parent.appendChild(joinElement);
}

QVector<const Table*> SelectQueryJoinComponent::getJoinedTables() const
{
    QVector<const Table*> result;
    for(int i=0; i<m_children.size(); i++)
    {
        TableQueryComponent* tc = dynamic_cast<TableQueryComponent*>(m_children.at(i));
        if(tc)
        {
            result.push_back(tc->getTable());
        }
    }
    return result;
}
