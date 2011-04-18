#include "SelectQueryJoinComponent.h"
#include "TableQueryComponent.h"
#include "strings.h"

SelectQueryJoinComponent::SelectQueryJoinComponent(QueryComponent* p, int l):QueryComponent(p,l)
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
        parent->removeJoin();
    }
}

QueryComponent* SelectQueryJoinComponent::duplicate()
{
    SelectQueryJoinComponent* newc = new SelectQueryJoinComponent(m_parent, m_level);
    return newc;
}
