#include "qbr_SelectQuerySelectComponent.h"
#include "qbr_SelectQuery.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_SelectQueryAsComponent.h"
#include "core_Column.h"
#include "core_Table.h"

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

CloneableElement* SelectQuerySelectComponent::clone(Version* sourceVersion, Version* targetVersion)
{
    SelectQuerySelectComponent* newc = dynamic_cast<SelectQuerySelectComponent*>(duplicate());
    newc->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, newc);
    return newc;
}

void SelectQuerySelectComponent::handleAction(const QString& action, QueryComponent* /*referringObject*/)
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

bool SelectQuerySelectComponent::hasGroupByFunctions()
{
    for(int i=0; i<m_children.size(); i++)
    {
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_children.at(i));
        if(seq)
        {
            if(seq->hasGroupByFunctions()) return true;
        }
    }
    return false;
}

bool SelectQuerySelectComponent::hasAtLeastOneColumnSelected()
{
    for(int i=0; i<m_children.size(); i++)
    {
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_children.at(i));
        if(seq)
        {
            if(seq->hasAtLeastOneColumnSelected()) return true;
        }
    }
    return false;
}

QVector<const Column*> SelectQuerySelectComponent::getSelectedColumns()
{
    QVector<const Column*> result;
    for(int i=0; i<m_children.size(); i++)
    {
        QVector<const Column*> localResult;
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_children.at(i));
        if(seq)
        {
            if(seq->hasAtLeastOneColumnSelected())
            {
                localResult = seq->getColumns();
                result += localResult;
            }
        }
    }
    return result;
}

QVector<const QueryComponent*> SelectQuerySelectComponent::getSelectedComponents()
{
    QVector<const QueryComponent*> result;
    for(int i=0; i<m_children.size(); i++)
    {
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_children.at(i));
        if(seq)
        {
            result.append(seq);
        }
    }
    return result;
}


QStringList SelectQuerySelectComponent::getOrderByElements()
{
    QStringList result;
    for(int i=0; i<m_children.size(); i++)
    {
        bool added = false;
        QVector<const Column*> columns;
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(m_children.at(i));
        if(seq)
        {
            if(seq->hasAtLeastOneColumnSelected())
            {
                columns = seq->getColumns();
            }

            for(int j=0; j<columns.size(); j++)
            {
                result.push_back(QString("$") + columns.at(j)->getTable()->getName() + "." + columns.at(j)->getName());  // add the columns
                added = true;
            }

            if(const SelectQueryAsComponent* as = seq->hasAs())             // and the last one is added as the alias
            {
                result.push_back(QString("~") + as->getAs());
                added = true;
            }
        }

        if(!added)
        {
            QString n;n.setNum(i + 1);
            result.push_back(QString("#") + n);
        }

    }

    return result;
}

QString SelectQuerySelectComponent::get() const
{
    QString result = "SELECT";
    if(m_children.size()) result += "\n";
    for(int i=0; i<m_children.size(); i++)
    {
        result += getSpacesForLevel();
        result+= m_children.at(i)->get();
        if(i<m_children.size() - 1) result += ",\n";
    }
    return result;
}

void SelectQuerySelectComponent::serialize(QDomDocument& doc, QDomElement& parent) const
{
    QDomElement selectElement = doc.createElement("Select");
    QueryComponent::serialize(doc, selectElement);
    parent.appendChild(selectElement);
}

QUuid SelectQuerySelectComponent::getClassUid() const
{
    return QUuid(uidSelectQuerySelectComponent);
}
