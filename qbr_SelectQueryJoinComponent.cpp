#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_WhereExpressionQueryComponent.h"
#include "qbr_SingleExpressionQueryComponent.h"

#include "strings.h"

SelectQueryJoinComponent::SelectQueryJoinComponent(QueryComponent* p, int l):QueryComponent(p,l), m_joinExpressions()
{
}

void SelectQueryJoinComponent::handleAction(const QString& action, QueryComponent* referringObject)
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
        WhereExpressionQueryComponent* c = new SingleExpressionQueryComponent(this, m_level);
        m_joinExpressions.append(c);
    }

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
    // TODO: Duplicate the join expressions vector
    return newc;
}
