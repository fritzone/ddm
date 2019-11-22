#include "qbr_SelectQuerySelectComponent.h"
#include "qbr_SelectQuery.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_SelectQueryAsComponent.h"
#include "core_Column.h"
#include "core_Table.h"
#include "core_TableInstance.h"

#include "ddm_strings.h"

SelectQuerySelectComponent::SelectQuerySelectComponent(Query* q, QueryComponent* p,
                                                       int l, Version *v) :
    QueryComponent(q, p, l, v)
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
    if(hasParent())
    {
        getParent()->onClose();
    }
}

QueryComponent* SelectQuerySelectComponent::duplicate()
{
    SelectQuerySelectComponent* newc = new SelectQuerySelectComponent(getQuery(), getParent(), getLevel(), version());
    return newc;
}

CloneableElement* SelectQuerySelectComponent::clone(Version* sourceVersion, Version* targetVersion)
{
    SelectQuerySelectComponent* newc = new SelectQuerySelectComponent(getQuery(), getParent(), getLevel(), targetVersion);
    newc->setSourceUid(getObjectUid());
    cloneTheChildren(sourceVersion, targetVersion, newc);
    return newc;
}

void SelectQuerySelectComponent::handleAction(const QString& action, QueryComponent* /*referringObject*/)
{
    if(action == ADD_WHERE_EXPRESSION)
    {
        if(getParent()->is<SelectQuery>())
        {
            getParent()->as<SelectQuery>()->newSelectExpression();
        }
    }
}

bool SelectQuerySelectComponent::hasGroupByFunctions()
{
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        if(children[i]->is<SingleExpressionQueryComponent>())
        {
            if(children[i]->as<SingleExpressionQueryComponent>()->hasGroupByFunctions())
            {
                return true;
            }
        }
    }
    return false;
}

bool SelectQuerySelectComponent::hasAtLeastOneColumnSelected()
{
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        if(children[i]->is<SingleExpressionQueryComponent>())
        {
            if(children[i]->as<SingleExpressionQueryComponent>()->hasAtLeastOneColumnSelected())
            {
                return true;
            }
        }
    }
    return false;
}

QVector<const ColumnOfTabWithTabInstance*> SelectQuerySelectComponent::getSelectedColumns()
{
    QVector<const ColumnOfTabWithTabInstance*> result;
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        QVector<const ColumnOfTabWithTabInstance*> localResult;
        if(children[i]->is<SingleExpressionQueryComponent>())
        {
            if(children[i]->as<SingleExpressionQueryComponent>()->hasAtLeastOneColumnSelected())
            {
                localResult = children[i]->as<SingleExpressionQueryComponent>()->getColumns();
                result += localResult;
            }
        }
    }
    return result;
}

QVector<const QueryComponent*> SelectQuerySelectComponent::getSelectedComponents()
{
    QVector<const QueryComponent*> result;
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        if(children[i]->is<SingleExpressionQueryComponent>())
        {
            result.append(children[i]);
        }
    }
    return result;
}


QStringList SelectQuerySelectComponent::getOrderByElements()
{
    QStringList result;
    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        bool somethingFound = false; // if something was added or not
        SingleExpressionQueryComponent* seq = dynamic_cast<SingleExpressionQueryComponent*>(children[i]);
        if(seq)
        {
            QVector<const ColumnOfTabWithTabInstance*> columns;

            if(seq->hasAtLeastOneColumnSelected())
            {
                columns = seq->getColumns();
            }

            for(int j=0; j<columns.size(); j++)
            {
                QString strTmp = strDollar;

                if(columns[j]->tab)
                {
                    strTmp += columns[j]->tab->getName();
                }
                else
                if(columns[j]->tinst)
                {
                    strTmp += columns[j]->tinst->getName();
                }

                strTmp += strDot + columns[j]->c->getName();

                result.push_back(strTmp);  // add the columns
                somethingFound = true;
            }

            if(const SelectQueryAsComponent* as = seq->hasAs())             // and the last one is added as the alias
            {
                result.push_back(strTilde + as->getAs());
                somethingFound = true;
            }
        }

        if(!somethingFound)
        {
            QString n;
            n.setNum(i + 1);
            result.push_back(strHash + n);
        }

    }

    return result;
}

QString SelectQuerySelectComponent::get() const
{
    QString result = "SELECT";

    if(hasChildren())
    {
        result += strNewline;
    }

    const QList<QueryComponent*>& children = getChildren();

    for(int i=0; i<children.size(); i++)
    {
        result += getSpacesForLevel();
        result+= children[i]->get();
        if(i < children.size() - 1)
        {
            result += strComma + strNewline;
        }
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
