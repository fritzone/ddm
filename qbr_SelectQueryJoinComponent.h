#ifndef SELECTQUERYJOINCOMPONENT_H
#define SELECTQUERYJOINCOMPONENT_H

#include "qbr_QueryComponents.h"
#include "qbr_QueryGraphicsHelper.h"

class SingleExpressionQueryComponent;
class Table;

class SelectQueryJoinComponent : public QueryComponent
{
public:
    SelectQueryJoinComponent(QueryComponent*, int);
    virtual QString get() const;
    virtual QString getClass() const {return "SelectQueryJoinComponent";}
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();
    virtual QueryComponent* duplicate();

    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }

    const QVector<SingleExpressionQueryComponent*>& getJoinExpresions() const
    {
        return m_joinExpressions;
    }

    void setHelper(QueryGraphicsHelper* h) {m_helper = h;}
    void removeExpression(SingleExpressionQueryComponent*);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    QVector<const Table*> getJoinedTables() const;
    void addJoinExpression(SingleExpressionQueryComponent* je)
    {
        m_joinExpressions.append(je);
    }

private:
    QueryGraphicsHelper* m_helper;
    QVector<SingleExpressionQueryComponent*> m_joinExpressions;
};

#endif // SELECTQUERYJOINCOMPONENT_H
