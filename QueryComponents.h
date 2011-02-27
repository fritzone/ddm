#ifndef QUERYCOMPONENTS_H
#define QUERYCOMPONENTS_H

#include <OptionsType.h>

#include <QString>
#include <QList>
#include <QSet>

class QueryGraphicsItem;
class QueryGraphicsHelper;

/**
 * The logic behind a query
 */
class QueryComponent
{
public:
    QueryComponent(QueryComponent* parent) : m_parent(parent) {}
    virtual ~QueryComponent(){}
    virtual QString get() = 0;
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*) = 0;
    void addChild(QueryComponent* c) {m_children.append(c);}
    QList<QueryComponent*>& getChildren() {return m_children;}
    void removeChild(QueryComponent* c);
    QueryComponent* getParent() {return m_parent; }
    virtual void handleAction(const QString& action) = 0;
    virtual QSet<OptionsType> provideOptions() = 0;
    void setParent(QueryComponent* p) {m_parent = p;}
protected:
    QList<QueryComponent*> m_children;
    QueryComponent* m_parent;
};

#endif // QUERYCOMPONENTS_H

