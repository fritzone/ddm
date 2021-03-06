#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "uids.h"
#include "ddm_strings.h"

SelectQueryJoinComponent::SelectQueryJoinComponent(Query* q, QueryComponent* p,
                                                   int l, Version *v) :
    QueryComponent(q, p, l, v),
    m_helper(0), m_joinExpressions()
{
}

void SelectQueryJoinComponent::handleAction(const QString& action, QueryComponent* /*referringObject*/)
{
    if(action == NEW_TABLE)
    {
        TableQueryComponent* tccp = TableQueryComponent::provideFirstTableIfAny(this, getLevel());
        if(!tccp) return;
        addChild(tccp);
        m_helper->triggerReRender();
        return;
    }
    if(action == ADD_WHERE_EXPRESSION)  // we get here when the user clicked the New join expr...
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(getQuery(), this, getLevel(), version());
        m_joinExpressions.append(c);
        m_helper->triggerReRender();
        return;
    }
    if(action == ADD_WHERE_EXPRESSION_AND)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(getQuery(), this, getLevel(), version());
        dynamic_cast<SingleExpressionQueryComponent*>(c)->setForcedType(SingleExpressionQueryComponent::FORCED_AND);
        m_joinExpressions.append(c);
        m_helper->triggerReRender();
        return;
    }
    if(action == ADD_WHERE_EXPRESSION_OR)
    {
        SingleExpressionQueryComponent* c = new SingleExpressionQueryComponent(getQuery(), this, getLevel(), version());
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
    TableQueryComponent* parent = dynamic_cast<TableQueryComponent*>(getParent());
    if(parent)
    {
        parent->removeJoin(this);
        m_helper->triggerReRender();
    }
}

QueryComponent* SelectQueryJoinComponent::duplicate()
{
    SelectQueryJoinComponent* newc = new SelectQueryJoinComponent(getQuery(), getParent(), getLevel(), version());
    for(int i=0; i<m_joinExpressions.size(); i++)
    {
        newc->m_joinExpressions.append(dynamic_cast<SingleExpressionQueryComponent*>(m_joinExpressions.at(i)->duplicate()));
    }
    return newc;
}

CloneableElement* SelectQueryJoinComponent::clone(Version *sourceVersion, Version *targetVersion)
{
    SelectQueryJoinComponent* newc = new SelectQueryJoinComponent(getQuery(), getParent(), getLevel(), targetVersion);
    for(int i=0; i<m_joinExpressions.size(); i++)
    {
        newc->m_joinExpressions.append(dynamic_cast<SingleExpressionQueryComponent*>(m_joinExpressions.at(i)->clone(sourceVersion, targetVersion)));
    }
    newc->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, newc);
    return newc;
}
void SelectQueryJoinComponent::removeExpression(SingleExpressionQueryComponent *w)
{
    m_joinExpressions.remove(m_joinExpressions.indexOf(w));
}

QString SelectQueryJoinComponent::get() const
{
    QString result = getSpacesForLevel() + "JOIN";

    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        result += getSpacesForLevel();
        result+= children.at(i)->get();
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
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        if(children[i]->is<TableQueryComponent>())
        {
            const Table* t = children[i]->as<TableQueryComponent>()->getTable();
            if(t)
            {
                result.push_back(t);
            }
        }
    }
    return result;
}

QVector<const TableInstance*> SelectQueryJoinComponent::getJoinedTableInstances() const
{
    QVector<const TableInstance*> result;
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        if(children[i]->is<TableQueryComponent>())
        {
            const TableInstance* ti = children[i]->as<TableQueryComponent>()->getTableInstance();
            if(ti)
            {
                result.push_back(ti);
            }
        }
    }
    return result;
}

QUuid SelectQueryJoinComponent::getClassUid() const
{
    return QUuid(uidSelectQueryJoinComponent);
}
