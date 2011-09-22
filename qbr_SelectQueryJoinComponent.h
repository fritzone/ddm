#ifndef SELECTQUERYJOINCOMPONENT_H
#define SELECTQUERYJOINCOMPONENT_H

#include "qbr_QueryComponents.h"
#include "qbr_QueryGraphicsHelper.h"

class WhereExpressionQueryComponent;

class SelectQueryJoinComponent : public QueryComponent
{
public:
    SelectQueryJoinComponent(QueryComponent*, int);
    virtual QString get() const;
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();
    virtual QueryComponent* duplicate();

    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }

    const QVector<WhereExpressionQueryComponent*>& getJoinExpresions() const
    {
        return m_joinExpressions;
    }

    void setHelper(QueryGraphicsHelper* h) {m_helper = h;}
    void removeExpression(WhereExpressionQueryComponent*);

private:
    QueryGraphicsHelper* m_helper;
    QVector<WhereExpressionQueryComponent*> m_joinExpressions;
};

#endif // SELECTQUERYJOINCOMPONENT_H
